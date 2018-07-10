//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_DETAIL_INTRUSIVE_LIST_HPP
#define MA_DETAIL_INTRUSIVE_LIST_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <algorithm>
#include <utility>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <ma/config.hpp>
#include <ma/detail/memory.hpp>

namespace ma {
namespace detail {

/// Simplified doubly linked intrusive list.
/**
 * Requirements:
 * if value is rvalue of type Value then expression
 * static_cast&lt;intrusive_list&lt;Value&gt;::base_hook&amp;&gt;(Value)
 * must be well formed and accessible from intrusive_list&lt;Value&gt;.
 */
template<typename Value>
class intrusive_list
{
private:
  typedef intrusive_list<Value> this_type;

public:
  typedef Value  value_type;
  typedef Value* pointer;
  typedef Value& reference;

  /// Required hook for items of the list.
  class base_hook;

  intrusive_list() MA_NOEXCEPT;

  intrusive_list(const this_type&) MA_NOEXCEPT;

  this_type& operator=(const this_type&) MA_NOEXCEPT;

#if defined(MA_HAS_RVALUE_REFS)

  intrusive_list(this_type&&) MA_NOEXCEPT;

  this_type& operator=(this_type&&) MA_NOEXCEPT;

#endif // defined(MA_HAS_RVALUE_REFS)

  pointer front() const MA_NOEXCEPT;

  pointer back() const MA_NOEXCEPT;

  static pointer prev(reference value) MA_NOEXCEPT;

  static pointer next(reference value) MA_NOEXCEPT;

  void push_front(reference value) MA_NOEXCEPT;

  void push_back(reference value) MA_NOEXCEPT;

  void erase(reference value) MA_NOEXCEPT;

  void pop_front() MA_NOEXCEPT;

  void pop_back() MA_NOEXCEPT;

  bool empty() const MA_NOEXCEPT;

  void clear() MA_NOEXCEPT;

  void swap(this_type&) MA_NOEXCEPT;

  void insert_front(this_type&) MA_NOEXCEPT;

  void insert_back(this_type&) MA_NOEXCEPT;

private:
  static base_hook& get_hook(reference value) MA_NOEXCEPT;

  pointer front_;
  pointer back_;
}; // class intrusive_list

template<typename Value>
class intrusive_list<Value>::base_hook
{
private:
  typedef base_hook this_type;

protected:
  base_hook() MA_NOEXCEPT;
  base_hook(const this_type&) MA_NOEXCEPT;
  base_hook& operator=(const this_type&) MA_NOEXCEPT;

private:
  friend class intrusive_list<Value>;
  pointer prev_;
  pointer next_;
}; // class intrusive_list::base_hook

/// Simplified singly linked intrusive list.
/**
 * Requirements:
 * if value is rvalue of type Value then expression
 * static_cast&lt;intrusive_forward_list&lt;Value&gt;::base_hook&amp;&gt;(Value)
 * must be well formed and accessible from intrusive_forward_list&lt;Value&gt;.
 */
template<typename Value>
class intrusive_forward_list
{
private:
  typedef intrusive_forward_list<Value> this_type;

public:
  typedef Value  value_type;
  typedef Value* pointer;
  typedef Value& reference;

  /// Required hook for items of the list.
  class base_hook;

  intrusive_forward_list() MA_NOEXCEPT;

  intrusive_forward_list(const this_type&) MA_NOEXCEPT;

  this_type& operator=(const this_type&) MA_NOEXCEPT;

#if defined(MA_HAS_RVALUE_REFS)

  intrusive_forward_list(this_type&&) MA_NOEXCEPT;

  this_type& operator=(this_type&&) MA_NOEXCEPT;

#endif // defined(MA_HAS_RVALUE_REFS)

  pointer front() const MA_NOEXCEPT;

  pointer back() const MA_NOEXCEPT;

  static pointer next(reference value) MA_NOEXCEPT;

  void push_front(reference value) MA_NOEXCEPT;

  void push_back(reference value) MA_NOEXCEPT;

  void pop_front() MA_NOEXCEPT;

  bool empty() const MA_NOEXCEPT;

  void clear() MA_NOEXCEPT;

  void swap(this_type&) MA_NOEXCEPT;

  void insert_front(this_type&) MA_NOEXCEPT;

  void insert_back(this_type&) MA_NOEXCEPT;

private:
  static base_hook& get_hook(reference value) MA_NOEXCEPT;

  pointer front_;
  pointer back_;
}; // class intrusive_forward_list

template<typename Value>
class intrusive_forward_list<Value>::base_hook
{
private:
  typedef base_hook this_type;

protected:
  base_hook() MA_NOEXCEPT;
  base_hook(const this_type&) MA_NOEXCEPT;
  base_hook& operator=(const this_type&) MA_NOEXCEPT;

private:
  friend class intrusive_forward_list<Value>;
  pointer next_;
}; // class intrusive_forward_list::base_hook

template<typename Value>
intrusive_list<Value>::base_hook::base_hook() MA_NOEXCEPT
  : prev_(0)
  , next_(0)
{
}

template<typename Value>
intrusive_list<Value>::base_hook::base_hook(const this_type&) MA_NOEXCEPT
  : prev_(0)
  , next_(0)
{
}

template<typename Value>
typename intrusive_list<Value>::base_hook&
intrusive_list<Value>::base_hook::operator=(const this_type&) MA_NOEXCEPT
{
  return *this;
}

template<typename Value>
intrusive_list<Value>::intrusive_list() MA_NOEXCEPT
  : front_(0)
  , back_(0)
{
}

template<typename Value>
intrusive_list<Value>::intrusive_list(const this_type& other) MA_NOEXCEPT
  : front_(other.front_)
  , back_(other.back_)
{
}

template<typename Value>
typename intrusive_list<Value>::this_type& intrusive_list<Value>::operator=(
    const this_type& other) MA_NOEXCEPT
{
  front_ = other.front_;
  back_  = other.back_;
  return *this;
}

#if defined(MA_HAS_RVALUE_REFS)

template<typename Value>
intrusive_list<Value>::intrusive_list(this_type&& other) MA_NOEXCEPT
  : front_(other.front_)
  , back_(other.back_)
{
  other.front_ = other.back_ = 0;
}

template<typename Value>
typename intrusive_list<Value>::this_type&
intrusive_list<Value>::operator=(this_type&& other) MA_NOEXCEPT
{
  front_ = other.front_;
  back_  = other.back_;
  other.front_ = other.back_ = 0;
  return *this;
}

#endif // defined(MA_HAS_RVALUE_REFS)

template<typename Value>
typename intrusive_list<Value>::pointer
intrusive_list<Value>::front() const MA_NOEXCEPT
{
  return front_;
}

template<typename Value>
typename intrusive_list<Value>::pointer
intrusive_list<Value>::back() const MA_NOEXCEPT
{
  return back_;
}

template<typename Value>
typename intrusive_list<Value>::pointer
intrusive_list<Value>::prev(reference value) MA_NOEXCEPT
{
  return get_hook(value).prev_;
}

template<typename Value>
typename intrusive_list<Value>::pointer
intrusive_list<Value>::next(reference value) MA_NOEXCEPT
{
  return get_hook(value).next_;
}

template<typename Value>
void intrusive_list<Value>::push_front(reference value) MA_NOEXCEPT
{
  base_hook& value_hook = get_hook(value);

  BOOST_ASSERT_MSG(!value_hook.prev_ && !value_hook.next_,
      "The value to push has to be unlinked");

  const pointer value_ptr = detail::addressof(value);

  value_hook.next_ = front_;
  if (value_hook.next_)
  {
    get_hook(*value_hook.next_).prev_ = value_ptr;
  }
  front_ = value_ptr;
  if (!back_)
  {
    back_ = front_;
  }

  BOOST_ASSERT_MSG(value_hook.prev_ || value_hook.next_ || (front_ == back_),
      "The pushed value has to be linked");
  BOOST_ASSERT_MSG(front_ && back_, "The list has to be not empty");
}

template<typename Value>
void intrusive_list<Value>::push_back(reference value) MA_NOEXCEPT
{
  base_hook& value_hook = get_hook(value);

  BOOST_ASSERT_MSG(!value_hook.prev_ && !value_hook.next_,
      "The value to push has to be unlinked");

  const pointer value_ptr = detail::addressof(value);

  value_hook.prev_ = back_;
  if (value_hook.prev_)
  {
    get_hook(*value_hook.prev_).next_ = value_ptr;
  }
  back_ = value_ptr;
  if (!front_)
  {
    front_ = back_;
  }

  BOOST_ASSERT_MSG(value_hook.prev_ || value_hook.next_ || (front_ == back_),
      "The pushed value has to be linked");
  BOOST_ASSERT_MSG(front_ && back_, "The list has to be not empty");
}

template<typename Value>
void intrusive_list<Value>::erase(reference value) MA_NOEXCEPT
{
  base_hook& value_hook = get_hook(value);
  const pointer value_ptr = detail::addressof(value);
  if (value_ptr == front_)
  {
    front_ = value_hook.next_;
  }
  if (value_ptr == back_)
  {
    back_ = value_hook.prev_;
  }
  if (value_hook.prev_)
  {
    get_hook(*value_hook.prev_).next_ = value_hook.next_;
  }
  if (value_hook.next_)
  {
    get_hook(*value_hook.next_).prev_ = value_hook.prev_;
  }
  value_hook.prev_ = value_hook.next_ = 0;

  BOOST_ASSERT_MSG(!value_hook.prev_ && !value_hook.next_,
      "The erased value has to be unlinked");
}

template<typename Value>
void intrusive_list<Value>::pop_front() MA_NOEXCEPT
{
  BOOST_ASSERT_MSG(front_, "The container is empty");

  base_hook& value_hook = get_hook(*front_);
  front_ = value_hook.next_;
  if (front_)
  {
    get_hook(*front_).prev_= 0;
  }
  else
  {
    back_ = 0;
  }
  value_hook.next_ = value_hook.prev_ = 0;

  BOOST_ASSERT_MSG(!value_hook.prev_ && !value_hook.next_,
      "The popped value has to be unlinked");
}

template<typename Value>
void intrusive_list<Value>::pop_back() MA_NOEXCEPT
{
  BOOST_ASSERT_MSG(back_, "The container is empty");

  base_hook& value_hook = get_hook(*back_);
  back_ = value_hook.prev_;
  if (back_)
  {
    get_hook(*back_).next_= 0;
  }
  else
  {
    front_ = 0;
  }
  value_hook.next_ = value_hook.prev_ = 0;

  BOOST_ASSERT_MSG(!value_hook.prev_ && !value_hook.next_,
      "The popped value has to be unlinked");
}

template<typename Value>
bool intrusive_list<Value>::empty() const MA_NOEXCEPT
{
  return !front_;
}

template<typename Value>
void intrusive_list<Value>::clear() MA_NOEXCEPT
{
  front_ = back_ = 0;
}

template<typename Value>
void intrusive_list<Value>::swap(this_type& other) MA_NOEXCEPT
{
  std::swap(front_, other.front_);
  std::swap(back_, other.back_);
}

template<typename Value>
void intrusive_list<Value>::insert_front(this_type& other) MA_NOEXCEPT
{
  if (other.empty())
  {
    return;
  }

  if (empty())
  {
    front_ = other.front_;
    back_  = other.back_;
    other.front_ = other.back_ = 0;
    return;
  }

  get_hook(*other.back_).next_ = front_;
  get_hook(*front_).prev_ = other.back_;
  front_ = other.front_;

  other.front_ = other.back_ = 0;

  BOOST_ASSERT_MSG(other.empty(), "The moved list has to be empty");
}

template<typename Value>
void intrusive_list<Value>::insert_back(this_type& other) MA_NOEXCEPT
{
  if (other.empty())
  {
    return;
  }

  if (empty())
  {
    front_ = other.front_;
    back_  = other.back_;
    other.front_ = other.back_ = 0;
    return;
  }

  get_hook(*back_).next_ = other.front_;
  get_hook(*other.front_).prev_ = back_;
  back_ = other.back_;

  other.front_ = other.back_ = 0;

  BOOST_ASSERT_MSG(other.empty(), "The moved list has to be empty");
}

template<typename Value>
typename intrusive_list<Value>::base_hook&
intrusive_list<Value>::get_hook(reference value) MA_NOEXCEPT
{
  return static_cast<base_hook&>(value);
}

template<typename Value>
intrusive_forward_list<Value>::base_hook::base_hook() MA_NOEXCEPT
  : next_(0)
{
}

template<typename Value>
intrusive_forward_list<Value>::base_hook::base_hook(
    const this_type&) MA_NOEXCEPT
  : next_(0)
{
}

template<typename Value>
typename intrusive_forward_list<Value>::base_hook&
intrusive_forward_list<Value>::base_hook::operator=(
    const this_type&) MA_NOEXCEPT
{
  return *this;
}

template<typename Value>
intrusive_forward_list<Value>::intrusive_forward_list() MA_NOEXCEPT
  : front_(0)
  , back_(0)
{
}

template<typename Value>
intrusive_forward_list<Value>::intrusive_forward_list(
    const this_type& other) MA_NOEXCEPT
  : front_(other.front_)
  , back_(other.back_)
{
}

template<typename Value>
typename intrusive_forward_list<Value>::this_type&
intrusive_forward_list<Value>::operator=(const this_type& other) MA_NOEXCEPT
{
  front_ = other.front_;
  back_  = other.back_;
  return *this;
}

#if defined(MA_HAS_RVALUE_REFS)

template<typename Value>
intrusive_forward_list<Value>::intrusive_forward_list(
    this_type&& other) MA_NOEXCEPT
  : front_(other.front_)
  , back_(other.back_)
{
  other.front_ = other.back_ = 0;
}

template<typename Value>
typename intrusive_forward_list<Value>::this_type&
intrusive_forward_list<Value>::operator=(this_type&& other) MA_NOEXCEPT
{
  front_ = other.front_;
  back_  = other.back_;
  other.front_ = other.back_ = 0;
  return *this;
}

#endif // defined(MA_HAS_RVALUE_REFS)

template<typename Value>
typename intrusive_forward_list<Value>::pointer
intrusive_forward_list<Value>::front() const MA_NOEXCEPT
{
  return front_;
}

template<typename Value>
typename intrusive_forward_list<Value>::pointer
intrusive_forward_list<Value>::back() const MA_NOEXCEPT
{
  return back_;
}

template<typename Value>
typename intrusive_forward_list<Value>::pointer
intrusive_forward_list<Value>::next(reference value) MA_NOEXCEPT
{
  return get_hook(value).next_;
}

template<typename Value>
void intrusive_forward_list<Value>::push_front(reference value) MA_NOEXCEPT
{
  base_hook& value_hook = get_hook(value);

  BOOST_ASSERT_MSG(!value_hook.next_, "The value to push has to be unlinked");

  value_hook.next_ = front_;
  front_ = detail::addressof(value);
  if (!back_)
  {
    back_ = front_;
  }

  BOOST_ASSERT_MSG(value_hook.next_ || (front_ == back_),
      "The pushed value has to be linked");
  BOOST_ASSERT_MSG(front_ && back_, "The list has to be not empty");
}

template<typename Value>
void intrusive_forward_list<Value>::push_back(reference value) MA_NOEXCEPT
{
  base_hook& value_hook = get_hook(value);

  BOOST_ASSERT_MSG(!value_hook.next_, "The value to push has to be unlinked");

  const pointer value_ptr = detail::addressof(value);
  if (back_)
  {
    get_hook(*back_).next_ = value_ptr;
  }
  back_ = value_ptr;
  if (!front_)
  {
    front_ = back_;
  }

  BOOST_ASSERT_MSG(value_hook.next_ || (front_ == back_),
      "The pushed value has to be linked");
  BOOST_ASSERT_MSG(front_ && back_, "The list has to be not empty");
}

template<typename Value>
void intrusive_forward_list<Value>::pop_front() MA_NOEXCEPT
{
  BOOST_ASSERT_MSG(front_, "The container is empty");

  base_hook& value_hook = get_hook(*front_);
  front_ = value_hook.next_;
  if (!front_)
  {
    back_ = 0;
  }
  value_hook.next_ = 0;

  BOOST_ASSERT_MSG(!value_hook.next_, "The popped value has to be unlinked");
}

template<typename Value>
bool intrusive_forward_list<Value>::empty() const MA_NOEXCEPT
{
  return !front_;
}

template<typename Value>
void intrusive_forward_list<Value>::clear() MA_NOEXCEPT
{
  front_ = back_ = 0;
}

template<typename Value>
void intrusive_forward_list<Value>::swap(this_type& other) MA_NOEXCEPT
{
  std::swap(front_, other.front_);
  std::swap(back_, other.back_);
}

template<typename Value>
void intrusive_forward_list<Value>::insert_front(
    this_type& other) MA_NOEXCEPT
{
  if (other.empty())
  {
    return;
  }

  if (empty())
  {
    front_ = other.front_;
    back_  = other.back_;
    other.front_ = other.back_ = 0;
    return;
  }

  get_hook(*other.back_).next_ = front_;
  front_ = other.front_;

  other.front_ = other.back_ = 0;

  BOOST_ASSERT_MSG(other.empty(), "The moved list has to be empty");
}

template<typename Value>
void intrusive_forward_list<Value>::insert_back(this_type& other) MA_NOEXCEPT
{
  if (other.empty())
  {
    return;
  }

  if (empty())
  {
    front_ = other.front_;
    back_  = other.back_;
    other.front_ = other.back_ = 0;
    return;
  }

  get_hook(*back_).next_ = other.front_;
  back_ = other.back_;

  other.front_ = other.back_ = 0;

  BOOST_ASSERT_MSG(other.empty(), "The moved list has to be empty");
}

template<typename Value>
typename intrusive_forward_list<Value>::base_hook&
intrusive_forward_list<Value>::get_hook(reference value) MA_NOEXCEPT
{
  return static_cast<base_hook&>(value);
}

} // namespace detail
} // namespace ma

#endif // MA_DETAIL_INTRUSIVE_LIST_HPP
