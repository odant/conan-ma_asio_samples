//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_ECHO_SERVER_SESSION_MANAGER_HPP
#define MA_ECHO_SERVER_SESSION_MANAGER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <ma/config.hpp>
#include <ma/detail/type_traits.hpp>
#include <ma/handler_storage.hpp>
#include <ma/handler_allocator.hpp>
#include <ma/bind_handler.hpp>
#include <ma/context_alloc_handler.hpp>
#include <ma/strand.hpp>
#include <ma/sp_intrusive_list.hpp>
#include <ma/echo/server/session_fwd.hpp>
#include <ma/echo/server/session_factory_fwd.hpp>
#include <ma/echo/server/session_config.hpp>
#include <ma/echo/server/session_manager_config.hpp>
#include <ma/echo/server/session_manager_stats.hpp>
#include <ma/echo/server/session_manager_fwd.hpp>
#include <ma/detail/memory.hpp>
#include <ma/detail/functional.hpp>
#include <ma/detail/thread.hpp>
#include <ma/detail/utility.hpp>

namespace ma {
namespace echo {
namespace server {

class session_manager
  : private boost::noncopyable
  , public  detail::enable_shared_from_this<session_manager>
{
private:
  typedef session_manager this_type;

public:
  typedef boost::asio::ip::tcp protocol_type;

  // Note that session_io_service has to outlive io_service
  static session_manager_ptr create(boost::asio::io_service& io_service,
      session_factory& managed_session_factory,
      const session_manager_config& config);

  void reset(bool free_recycled_sessions = true);

  session_manager_stats stats();

  template <typename Handler>
  void async_start(MA_FWD_REF(Handler) handler);

  template <typename Handler>
  void async_stop(MA_FWD_REF(Handler) handler);

  template <typename Handler>
  void async_wait(MA_FWD_REF(Handler) handler);

protected:
  // Note that session_io_service has to outlive io_service
  session_manager(boost::asio::io_service&, session_factory&,
      const session_manager_config&);
  ~session_manager();

private:
  class stats_collector : private boost::noncopyable
  {
  private:
    typedef detail::mutex                  mutex_type;
    typedef detail::lock_guard<mutex_type> lock_guard_type;

  public:
    stats_collector();

    session_manager_stats stats();
    void set_active_session_count(std::size_t);
    void set_recycled_session_count(std::size_t);
    void session_accepted(const boost::system::error_code&);
    void session_stopped(const boost::system::error_code&);
    void reset();

  private:
    mutex_type mutex_;
    session_manager_stats stats_;
  }; // class stats_collector

  class session_wrapper_base
    : public sp_intrusive_list<session_wrapper_base>::base_hook
  {
  }; // class session_wrapper_base

  class session_wrapper;
  typedef detail::shared_ptr<session_wrapper> session_wrapper_ptr;
  typedef sp_intrusive_list<session_wrapper_base> session_list;

#if defined(MA_HAS_RVALUE_REFS) && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

  // Home-grown binders to support move semantic
  class accept_handler_binder;
  class session_dispatch_binder;
  class session_handler_binder;

  template <typename Arg>
  class forward_handler_binder;

#endif

  struct extern_state
  {
    enum value_t {ready, work, stop, stopped};
  };

  struct intern_state
  {
    enum value_t {work, stop, stopped};
  };

  struct accept_state
  {
    enum value_t {ready, in_progress, stopped};
  };

  typedef boost::optional<boost::system::error_code> optional_error_code;

  template <typename Handler>
  void start_extern_start(Handler&);

  template <typename Handler>
  void start_extern_stop(Handler&);

  template <typename Handler>
  void start_extern_wait(Handler&);

  void handle_accept(const session_wrapper_ptr&,
      const boost::system::error_code&);
  void handle_session_start(const session_wrapper_ptr&,
      const boost::system::error_code&);
  void handle_session_wait(const session_wrapper_ptr&,
      const boost::system::error_code&);
  void handle_session_stop(const session_wrapper_ptr&,
      const boost::system::error_code&);

  boost::system::error_code do_start_extern_start();
  optional_error_code do_start_extern_stop();
  optional_error_code do_start_extern_wait();
  void complete_extern_stop(const boost::system::error_code&);
  void complete_extern_wait(const boost::system::error_code&);

  void continue_work();

  void handle_accept_at_work(const session_wrapper_ptr&,
      const boost::system::error_code&);
  void handle_accept_at_stop(const session_wrapper_ptr&,
      const boost::system::error_code&);

  void handle_session_start_at_work(const session_wrapper_ptr&,
      const boost::system::error_code&);
  void handle_session_start_at_stop(const session_wrapper_ptr&,
      const boost::system::error_code&);

  void handle_session_wait_at_work(const session_wrapper_ptr&,
      const boost::system::error_code&);
  void handle_session_wait_at_stop(const session_wrapper_ptr&,
      const boost::system::error_code&);

  void handle_session_stop_at_work(const session_wrapper_ptr&,
      const boost::system::error_code&);
  void handle_session_stop_at_stop(const session_wrapper_ptr&,
      const boost::system::error_code&);

  void start_stop(const boost::system::error_code&);
  void continue_stop();

  session_wrapper_ptr start_active_session_stop(
      session_wrapper_ptr begin, std::size_t max_count);
  void schedule_active_session_stop();
  void handle_scheduled_active_session_stop();

  void start_accept_session(const session_wrapper_ptr&);
  void start_session_start(const session_wrapper_ptr&);
  void start_session_stop(const session_wrapper_ptr&);
  void start_session_wait(const session_wrapper_ptr&);

  void recycle(const session_wrapper_ptr&);
  session_wrapper_ptr create_session(boost::system::error_code& error);

  void add_to_active(const session_wrapper_ptr&);
  void add_to_recycled(const session_wrapper_ptr&);
  void remove_from_active(const session_wrapper_ptr&);
  void remove_from_recycled(const session_wrapper_ptr&);

  boost::system::error_code open_acceptor();
  boost::system::error_code close_acceptor();

  static void dispatch_handle_session_start(const session_manager_weak_ptr&,
      const session_wrapper_ptr&, const boost::system::error_code&);
  static void dispatch_handle_session_wait(const session_manager_weak_ptr&,
      const session_wrapper_ptr&, const boost::system::error_code&);
  static void dispatch_handle_session_stop(const session_manager_weak_ptr&,
      const session_wrapper_ptr&, const boost::system::error_code&);

  const protocol_type::endpoint accepting_endpoint_;
  const int                     listen_backlog_;
  const std::size_t             max_session_count_;
  const std::size_t             recycled_session_count_;
  const std::size_t             max_stopping_sessions_;
  const session_config          managed_session_config_;

  extern_state::value_t extern_state_;
  intern_state::value_t intern_state_;
  accept_state::value_t accept_state_;
  std::size_t           pending_operations_;

  boost::asio::io_service&  io_service_;
  session_factory&          session_factory_;
  ma::strand                strand_;
  protocol_type::acceptor   acceptor_;
  session_list              active_sessions_;
  session_list              recycled_sessions_;
  session_wrapper_ptr       stopping_sessions_end_;
  boost::system::error_code accept_error_;
  boost::system::error_code extern_wait_error_;
  stats_collector           stats_collector_;

  handler_storage<boost::system::error_code> extern_wait_handler_;
  handler_storage<boost::system::error_code> extern_stop_handler_;

  in_place_handler_allocator<512> accept_allocator_;
  in_place_handler_allocator<256> session_stop_allocator_;
}; // class session_manager

#if defined(MA_HAS_RVALUE_REFS) && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

template <typename Arg>
class session_manager::forward_handler_binder
{
private:
  typedef forward_handler_binder this_type;

public:
  typedef void result_type;
  typedef void (session_manager::*func_type)(Arg&);

  template <typename SessionManagerPtr>
  forward_handler_binder(func_type func, SessionManagerPtr&& session_manager);

#if defined(MA_NO_IMPLICIT_MOVE_CONSTRUCTOR) || !defined(NDEBUG)

  forward_handler_binder(this_type&&);
  forward_handler_binder(const this_type&);

#endif // defined(MA_NO_IMPLICIT_MOVE_CONSTRUCTOR) || !defined(NDEBUG)

  void operator()(Arg& arg);

private:
  func_type func_;
  session_manager_ptr session_manager_;
}; // class session_manager::forward_handler_binder

#endif // defined(MA_HAS_RVALUE_REFS)
       //     && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

template <typename Handler>
void session_manager::async_start(MA_FWD_REF(Handler) handler)
{
  typedef typename detail::decay<Handler>::type handler_type;
  typedef void (this_type::*func_type)(handler_type&);
  func_type func = &this_type::start_extern_start<handler_type>;

#if defined(MA_HAS_RVALUE_REFS) && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

  strand_.post(make_explicit_context_alloc_handler(
      detail::forward<Handler>(handler),
      forward_handler_binder<handler_type>(func, shared_from_this())));

#else

  strand_.post(make_explicit_context_alloc_handler(
      detail::forward<Handler>(handler),
      detail::bind(func, shared_from_this(), detail::placeholders::_1)));

#endif
}

template <typename Handler>
void session_manager::async_stop(MA_FWD_REF(Handler) handler)
{
  typedef typename detail::decay<Handler>::type handler_type;
  typedef void (this_type::*func_type)(handler_type&);
  func_type func = &this_type::start_extern_stop<handler_type>;

#if defined(MA_HAS_RVALUE_REFS) && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

  strand_.post(make_explicit_context_alloc_handler(
      detail::forward<Handler>(handler),
      forward_handler_binder<handler_type>(func, shared_from_this())));

#else

  strand_.post(make_explicit_context_alloc_handler(
      detail::forward<Handler>(handler),
      detail::bind(func, shared_from_this(), detail::placeholders::_1)));

#endif
}

template <typename Handler>
void session_manager::async_wait(MA_FWD_REF(Handler) handler)
{
  typedef typename detail::decay<Handler>::type handler_type;
  typedef void (this_type::*func_type)(handler_type&);
  func_type func = &this_type::start_extern_wait<handler_type>;

#if defined(MA_HAS_RVALUE_REFS) && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

  strand_.post(make_explicit_context_alloc_handler(
      detail::forward<Handler>(handler),
      forward_handler_binder<handler_type>(func, shared_from_this())));

#else

  strand_.post(make_explicit_context_alloc_handler(
      detail::forward<Handler>(handler),
      detail::bind(func, shared_from_this(), detail::placeholders::_1)));

#endif
}

inline session_manager::~session_manager()
{
}

template <typename Handler>
void session_manager::start_extern_start(Handler& handler)
{
  boost::system::error_code error = do_start_extern_start();
  io_service_.post(ma::bind_handler(detail::move(handler), error));
}

template <typename Handler>
void session_manager::start_extern_stop(Handler& handler)
{
  if (optional_error_code result = do_start_extern_stop())
  {
    io_service_.post(ma::bind_handler(detail::move(handler), *result));
  }
  else
  {
    extern_stop_handler_.store(detail::move(handler));
  }
}

template <typename Handler>
void session_manager::start_extern_wait(Handler& handler)
{
  if (optional_error_code result = do_start_extern_wait())
  {
    io_service_.post(ma::bind_handler(detail::move(handler), *result));
  }
  else
  {
    extern_wait_handler_.store(detail::move(handler));
  }
}

#if defined(MA_HAS_RVALUE_REFS) && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

template <typename Arg>
template <typename SessionManagerPtr>
session_manager::forward_handler_binder<Arg>::forward_handler_binder(
    func_type func, SessionManagerPtr&& session_manager)
  : func_(func)
  , session_manager_(detail::forward<SessionManagerPtr>(session_manager))
{
}

#if defined(MA_NO_IMPLICIT_MOVE_CONSTRUCTOR) || !defined(NDEBUG)

template <typename Arg>
session_manager::forward_handler_binder<Arg>::forward_handler_binder(
    this_type&& other)
  : func_(other.func_)
  , session_manager_(detail::move(other.session_manager_))
{
}

template <typename Arg>
session_manager::forward_handler_binder<Arg>::forward_handler_binder(
    const this_type& other)
  : func_(other.func_)
  , session_manager_(other.session_manager_)
{
}

#endif // defined(MA_NO_IMPLICIT_MOVE_CONSTRUCTOR) || !defined(NDEBUG)

template <typename Arg>
void session_manager::forward_handler_binder<Arg>::operator()(Arg& arg)
{
  ((*session_manager_).*func_)(arg);
}

#endif // defined(MA_HAS_RVALUE_REFS)
       //     && defined(MA_BIND_HAS_NO_MOVE_CONSTRUCTOR)

} // namespace server
} // namespace echo
} // namespace ma

#endif // MA_ECHO_SERVER_SESSION_MANAGER_HPP
