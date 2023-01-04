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

template <class T>
class Owned {

  friend class Mutexed<T>;
  friend class Mutexed<std::remove_const_t<T>>;

  T& obj_;
  std::unique_lock<std::mutex> lock_;

  Owned(T& obj, std::unique_lock<std::mutex>&& lock)
      : obj_(obj), lock_(std::move(lock)) {}


 public:
  T* operator->() const { return &obj_; }
  T& operator*() const { return obj_; }
};


template <class T, class Mutex = std::mutex>
class Mutexed : public {

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

  Owned<T> own() {
    std::unique_lock<std::mutex> lock(mutex_);
    return Owned<T>(obj_, std::move(lock));
  }

  Owned<const T> own() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return Owned<const T>(obj_, std::move(lock));
  }

  /// Returns the const version
  /// Useful for borrowing in a const fashion from a non-const Mutexed
  Owned<const T> cown() const {
    return own();
  }

  /// Useful for condition variables and the like
  std::mutex& get_mutex() const {
    return mutex_;
  }

  std::optional<Owned<T>> try_own() {
    if (auto lock = std::unique_lock<std::mutex>(mutex_, std::try_to_lock)) {
      return Owned<T>(obj_, std::move(lock));
    } else {
      return {};
    }
  }
};

}  // namespace mutexed
