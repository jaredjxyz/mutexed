/**
 * Copyright Jared Jensen, 2020 and all "mutexed" contributors
 * https://github.com/jaredjxyz/mutexed
 **/

#include <memory>
#include <mutex>
#include <utility>
#include <type_traits>
#include <optional>

namespace mutexed {

template<class T, class Mutex>
class Mutexed;

template <class T, class Mutex = std::mutex>
class Owned {

  template<class, class> friend class Mutexed;

  T& obj_;
  std::unique_lock<Mutex> lock_;

  Owned(T& obj, std::unique_lock<Mutex>&& lock)
      : obj_(obj), lock_(std::move(lock)) {}


 public:
  T* operator->() const { return &obj_; }
  T& operator*() const { return obj_; }
};


template <class T, class Mutex = std::mutex>
class Mutexed {

  mutable Mutex mutex_;
  T obj_;

 public:
  template<class... Args>
  Mutexed(Args&&... args) : obj_(std::forward<Args>(args)...) {}

  // Let's just make these explicit
  // Maybe one day make it copyable and movable
  // Avoiding deadlock when locking both mutexes while only in the initializer list
  // is slightly tricky but could be a fun problem to solve
  Mutexed(const Mutexed&) = delete;
  Mutexed(Mutexed&&) = delete;
  Mutexed& operator=(const Mutexed&) = delete;
  Mutexed& operator=(Mutexed&&) = delete;
  ~Mutexed() = default;

  Owned<T, Mutex> own() {
    std::unique_lock<Mutex> lock(mutex_);
    return Owned<T, Mutex>(obj_, std::move(lock));
  }

  Owned<const T, Mutex> own() const {
    std::unique_lock<Mutex> lock(mutex_);
    return Owned<const T, Mutex>(obj_, std::move(lock));
  }

  /// Returns the const version
  /// Useful for borrowing in a const fashion from a non-const Mutexed
  Owned<const T, Mutex> cown() const {
    return own();
  }

  /// Useful for condition variables and the like
  Mutex& get_mutex() const {
    return mutex_;
  }

  std::optional<Owned<T, Mutex>> try_own() {
    if (auto lock = std::unique_lock<Mutex>(mutex_, std::try_to_lock)) {
      return Owned<T, Mutex>(obj_, std::move(lock));
    } else {
      return {};
    }
  }
};

}  // namespace mutexed
