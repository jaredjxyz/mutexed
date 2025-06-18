# Mutexed: a tiny C++ tool to make writing thread-safe code easier

Making multithreaded code safe is hard.

Making sure you don't actually change your safe multithreaded code to make it unsafe is even harder.

## TLDR

Let's say we have a class CriticalResources that is not thread-safe but needs to be accessed by multiple threads in a thread-safe way.
```
struct CriticalResources {
  Printer printer;
  int i;
}
```
the old way:
```
class ThreadSafeResources {
...
std::mutex mutex_;
CriticalResources resources_;  // protected by mutex_

  void set_i(int i, const std::string& s) {
    std::lock_guard<std::mutex> lock(mutex_);
    resources_.i = i;
    resources_.printer.print(s);
  }
...
}
```
Using Mutexed:
```
class ThreadSafeResources {
  ...
  Mutexed<CriticalResources> resources_;

  void set_i(int i, const std::string& s) {
    Owned<CriticalResources> resources = resources_.own();
    resources->i = i;
    resources->printer.print(s);
  }
  ...
}
```

Mutexed hides access to any content that needs to be secured by a mutex (the critical section), and only gives access to that content after it has secured the mutex, and then unlocks the mutex when that content goes out of scope. And it does so with very little overhead.

## What's wrong with the "Old way"?

Having your lock next to the variables it protects and relying on remembering to lock the mutex at the right time, has two issues:
1. Often developers don't bother commenting to show what objects are protected by mutexes and which are not, so it's difficult to know what exactly the critical section is.
2. It's very easy for someone using this code to not realize they need to lock the mutex if they didn't read the comment, or to otherwise accidentally use the critical section without making sure they have exclusive access.

## What exists to solve this issue?

[Clang Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html) solves this pretty well I think.
And there are various ways to use information hiding and abstraction on a case-by-case basis.
I have not found a wrapper that does information hiding well on a general basis, and I think this solution handles this well.

If anyone finds another project that does the same thing or similar, please point me to it! I'm sure this is not a totally novel idea, but I wasn't able to find any open source examples. My afternoon of Googling came up with nothing, which is why I wrote this myself.

## Borrowed ownership is a simple, intuitive, and useful concept
Taking a page from the ownership model introduced with C++11 and smart pointers (Single-ownership for unique_ptr and shared ownership for shared_ptr), I introduce here the idea of borrowed ownership.

The idea of borrowed ownership is pretty simple. Imagine I live in a neighborhood with neighbors that often want to borrow my powertools.
```
Mutexed<PowerTool> my_powertool(PowerTool(...));
```

I am always the owner of that powertool, but I may not have possession of it at any one time. In addition, I may have multiple friends that want to use it at the same time.

If one of my friends, let's call him thread1, wants to use my powertool, they will have to borrow it.
```
// Thread1
Owned<PowerTool> powertool = my_powertool.own();
// Access the powertool by dereferencing with * or ->
```

This grants thread1 exclusive access to the PowerTool for as long as it needs it. When `powertool` goes out of scope and its destructor is called, thread 1 releases its use of PowerTool automatically, and the next thread waiting for the PowerTool borrows it.

Let's say I have another friend, thread2, that also wants to borrow `my_powertool`, but thread1 is currently using it and modifying it. Thread2 calls `Owned<PowerTool> thread2_powertool = my_powertool.own()`. But since thread1 currently has access, thread2 will wait at this point until thread1 is finished with PowerTool.

It's just that simple!

## Caveats
- If the Mutexed object is destroyed before the Owned object, that causes undefined behavior. The Mutexed object needs to always be alive in order for the Owned object to use the resource.
- In the current implementation, the resource is stored on the heap with a dynamic allocation. If that isn't okay for your program, you may want to change that behavior.

## It's got buzzwords!
- RAII: access to the critical section is revoked when the Owned goes out of scope, which is also when the mutex is unlocked.
- Smart pointers: The Owned object is a pointer-like object: use -> to access the members of the critical section and use * to dereference it to get the critical section!
- Synchronization: Access to the Mutexed object is thread-safe!

## Usage

`cd` into the root folder of this repo. You can build and install with `mkdir build && cd build && cmake .. && sudo make install`.

## Running tests

The CMake build optionally compiles a small test suite. CMake will search for
an installed copy of Google Test. If it is not found, the build system uses
`FetchContent` to automatically download Google Test. Systems without internet
access should install `libgtest-dev` or provide `GTest_ROOT` so CMake can find
the library. Tests can also be disabled entirely with `-DBUILD_TESTS=OFF`.


## Status

I have no intention of building this into a fully-featured suite. It has basic functionality and that's it. I'm open to pull requests if anyone wants to improve it!

Cool features to add would be:
- Do performance analysis to see if this could be sped up or to see how big the slowdown is, if there is one at all.
- Think about if it would be useful to allow different mutex types
- Add the ability to check if the resource is currently loaned out, instead of just trying to use it (the equivalent of try_lock)
- Add more programming languages
- Multi-level borrowing: Give neighbors the ability to loan out the powertool they loaned.
