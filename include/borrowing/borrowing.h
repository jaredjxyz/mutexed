/**
 * Copyright Jared Jensen, 2020 and all "borrowing" contributors
 * https://github.com/jaredjxyz/borrowing
 **/

#include <memory>
#include <mutex>
#include <utility>
#include <type_traits>

#if __cplusplus >= 201703L
#include <optional>
#endif // __cplusplus >= 201703L

namespace borrowing {

template<class T>
class Borrowable;

template <class T>
class Borrowed {

  friend class Borrowable<T>;
  friend class Borrowable<std::remove_const_t<T>>;

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

  mutable std::mutex mutex_;
  T obj_;

 public:
  template<class... Args>
  Borrowable(Args&&... args) : obj_(std::forward<Args>(args)...) {}

  // Let's just make these explicit
  // Maybe one day make it copyable and movable
  // Avoiding deadlock when locking both mutexes while only in the initializer list
  // is slightly tricky but could be a fun problem to solve
  Borrowable(const Borrowable&) = delete;
  Borrowable(Borrowable&&) = delete;
  Borrowable& operator=(const Borrowable&) = delete;
  Borrowable& operator=(Borrowable&&) = delete;
  ~Borrowable() = default;

  Borrowed<T> borrow() {
    std::unique_lock<std::mutex> lock(mutex_);
    return Borrowed<T>(obj_, std::move(lock));
  }

  Borrowed<const T> borrow() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return Borrowed<const T>(obj_, std::move(lock));
  }

  /// Returns the const version
  /// Useful for borrowing in a const fashion from a non-const Borrowable
  Borrowed<const T> cborrow() const {
    return borrow();
  }

  /// Useful for condition variables and the like
  std::mutex& get_mutex() const {
    return mutex_;
  }

  /// Enable try_... if we have optional
#if __cplusplus >= 201703L
  std::optional<Borrowed<T>> try_borrow() {
    if (auto lock = std::unique_lock<std::mutex>(mutex_, std::try_to_lock)) {
      return Borrowed<T>(obj_, std::move(lock));
    } else {
      return {};
    }
  }

  std::optional<Borrowed<const T>> try_borrow() const {
    if (auto lock = std::unique_lock<std::mutex>(mutex_, std::try_to_lock)) {
      return Borrowed<const T>(obj_, std::move(lock));
    } else {
      return {};
    }
  }

  /// Returns the const version.
  /// Useful for borrowing in a const fashion from a non-const Borrowable
  std::optional<Borrowed<const T>> try_cborrow() const {
    return try_borrow();
  }
#endif  // __cplusplus >= 201703L
};

}  // namespace borrowing
