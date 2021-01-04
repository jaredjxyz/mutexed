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

  T* obj_ptr_;
  std::unique_lock<std::mutex> lock_;

  Borrowed(T* obj_ptr, std::unique_lock<std::mutex>&& lock)
      : obj_ptr_(obj_ptr), lock_(std::move(lock)) {}


 public:
  T* operator->() const { return obj_ptr_; }
  T& operator*() const { return *obj_ptr_; }
};


template <class T>
class Borrowable {
  std::unique_ptr<T> obj_;
  std::mutex mutex_;

 public:
  Borrowable(std::unique_ptr<T>&& obj) : obj_(std::move(obj)) {}
  Borrowable(const T& obj) : obj_(std::make_unique<T>(obj)) {};
  Borrowable(T&& obj) : obj_(std::make_unique<T>(std::move(obj))) {}

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
