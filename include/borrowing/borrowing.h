#include <memory>
#include <condition_variable>
#include <memory>
#include <thread>
#include <iostream>

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
};

}  // namespace borrowing
