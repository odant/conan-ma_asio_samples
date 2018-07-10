//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_SP_INTRUSIVE_LIST_HPP
#define MA_SP_INTRUSIVE_LIST_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <ma/config.hpp>
#include <ma/detail/memory.hpp>

namespace ma {

/// Simplified double-linked intrusive list of shared_ptr.
/**
 * Const time insertion of shared_ptr.
 * Const time deletion of shared_ptr (deletion by value).
 *
 * Requirements:
 * if value is rvalue of type Value then expression
 * static_cast&lt;sp_intrusive_list&lt;Value&gt;::base_hook&amp;&gt;(value)
 * must be well formed and accessible from sp_intrusive_list&lt;Value&gt;.
 */
template <typename Value>
class sp_intrusive_list : private boost::noncopyable
{
public:
  typedef Value  value_type;
  typedef Value* pointer;
  typedef Value& reference;
  typedef detail::weak_ptr<Value>   weak_pointer;
  typedef detail::shared_ptr<Value> shared_pointer;

  /// Required hook for items of the list.
  class base_hook;

  sp_intrusive_list() MA_NOEXCEPT;

  /// Throws if value_type destructor throws
  ~sp_intrusive_list();

  const shared_pointer& front() const MA_NOEXCEPT;

  static shared_pointer prev(const shared_pointer& value) MA_NOEXCEPT;

  static const shared_pointer& next(const shared_pointer& value) MA_NOEXCEPT;

  void push_front(const shared_pointer& value) MA_NOEXCEPT;

  void erase(const shared_pointer& value) MA_NOEXCEPT;

  /// Throws if value_type destructor throws
  void clear();

  std::size_t size() const MA_NOEXCEPT;

  bool empty() const MA_NOEXCEPT;

private:
  static base_hook& get_hook(reference value) MA_NOEXCEPT;

  std::size_t    size_;
  shared_pointer front_;
}; // class sp_intrusive_list

template <typename Value>
class sp_intrusive_list<Value>::base_hook
{
private:
  typedef base_hook this_type;

public:
  base_hook() MA_NOEXCEPT;
  base_hook(const this_type&) MA_NOEXCEPT;
  base_hook& operator=(const this_type&) MA_NOEXCEPT;

private:
  friend class sp_intrusive_list<Value>;
  weak_pointer   prev_;
  shared_pointer next_;
}; // class sp_intrusive_list::base_hook

template <typename Value>
sp_intrusive_list<Value>::base_hook::base_hook() MA_NOEXCEPT
{
}

template <typename Value>
sp_intrusive_list<Value>::base_hook::base_hook(const this_type&) MA_NOEXCEPT
{
}

template <typename Value>
typename sp_intrusive_list<Value>::base_hook&
sp_intrusive_list<Value>::base_hook::operator=(const this_type&) MA_NOEXCEPT
{
  return *this;
}

template <typename Value>
sp_intrusive_list<Value>::sp_intrusive_list() MA_NOEXCEPT
  : size_(0)
{
}

template <typename Value>
sp_intrusive_list<Value>::~sp_intrusive_list()
{
  clear();
}

template <typename Value>
const typename sp_intrusive_list<Value>::shared_pointer&
sp_intrusive_list<Value>::front() const MA_NOEXCEPT
{
  return front_;
}

template <typename Value>
typename sp_intrusive_list<Value>::shared_pointer
sp_intrusive_list<Value>::prev(const shared_pointer& value) MA_NOEXCEPT
{
  BOOST_ASSERT_MSG(value, "The value can no not be null ptr");
  return get_hook(*value).prev_.lock();
}

template <typename Value>
const typename sp_intrusive_list<Value>::shared_pointer&
sp_intrusive_list<Value>::next(const shared_pointer& value) MA_NOEXCEPT
{
  BOOST_ASSERT_MSG(value, "The value can no not be null ptr");
  return get_hook(*value).next_;
}

template <typename Value>
void sp_intrusive_list<Value>::push_front(
    const shared_pointer& value) MA_NOEXCEPT
{
  BOOST_ASSERT_MSG(value, "The value can no not be null ptr");

  base_hook& value_hook = get_hook(*value);

  BOOST_ASSERT_MSG(!value_hook.prev_.lock() && !value_hook.next_,
      "The value to push has to be not linked");

  value_hook.next_ = front_;
  if (front_)
  {
    base_hook& front_hook = get_hook(*front_);
    front_hook.prev_ = value;
  }
  front_ = value;
  ++size_;
}

template <typename Value>
void sp_intrusive_list<Value>::erase(const shared_pointer& value) MA_NOEXCEPT
{
  BOOST_ASSERT_MSG(value, "The value can no not be null ptr");

  base_hook& value_hook = get_hook(*value);
  if (value == front_)
  {
    front_ = value_hook.next_;
  }
  const shared_pointer prev = value_hook.prev_.lock();
  if (prev)
  {
    base_hook& prev_hook = get_hook(*prev);
    prev_hook.next_ = value_hook.next_;
  }
  if (value_hook.next_)
  {
    base_hook& next_hook = get_hook(*value_hook.next_);
    next_hook.prev_ = value_hook.prev_;
  }
  value_hook.prev_.reset();
  value_hook.next_.reset();
  --size_;

  BOOST_ASSERT_MSG(!value_hook.prev_.lock() && !value_hook.next_,
      "The erased value has to be unlinked");
}

template <typename Value>
void sp_intrusive_list<Value>::clear()
{
  // We don't want to have recursive calls of wrapped_session's destructor
  // because the deep of such recursion may be equal to the size of list.
  // The last can be too great for the stack.
  while (front_)
  {
    base_hook& front_hook = get_hook(*front_);
    shared_pointer tmp;
    tmp.swap(front_hook.next_);
    front_hook.prev_.reset();
    front_.swap(tmp);
  }
  size_ = 0;
}

template <typename Value>
std::size_t sp_intrusive_list<Value>::size() const MA_NOEXCEPT
{
  return size_;
}

template <typename Value>
bool sp_intrusive_list<Value>::empty() const MA_NOEXCEPT
{
  return 0 == size_;
}

template <typename Value>
typename sp_intrusive_list<Value>::base_hook&
sp_intrusive_list<Value>::get_hook(reference value) MA_NOEXCEPT
{
  return static_cast<base_hook&>(value);
}

} // namespace ma

#endif // MA_SP_INTRUSIVE_LIST_HPP
