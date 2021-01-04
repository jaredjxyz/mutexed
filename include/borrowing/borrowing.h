/**
 * Copyright Jared Jensen, 2020 and all "borrowing" contributors
 * https://github.com/jaredjxyz/borrowing
 **/

#include <memory>
#include <mutex>
#include <utility>

namespace borrowing {

template<class T>
class Borrowable;

template <class T>
class Borrowed {

  friend class Borrowable<T>;

  T& obj_;
  std::unique_lock<std::mutex> lock_;

  Borrowed(T& obj, std::unique_lock<std::mutex>&& lock)
      : obj_(obj), lock_(std::move(lock)) {}


 public:
  T* operator->() const { return &obj_; }
  T& operator*() const { return obj_; }
};


template <class T>
class Borrowable {
  T obj_;
  std::mutex mutex_;

 public:
  template<class... Args>
  Borrowable(Args&&... args) : obj_(std::forward<Args>(args)...) {}

  Borrowed<T> borrow() {
    std::unique_lock<std::mutex> lock(mutex_);
    return Borrowed<T>(obj_.get(), std::move(lock));
  }

  Borrowed<const T> borrow() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return Borrowed<const T>(obj_.get(), std::move(lock));
  }

  /// Returns the const version
  /// Useful for borrowing in a const fashion from a non-const Borrowable
  Borrowed<const T> cborrow() const {
    return borrow();
  }
};

}  // namespace borrowing
