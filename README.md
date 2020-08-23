# Borrowing.h: a tiny C++ tool to make writing thread-safe code easier

Making multithreaded code safe is hard.

Making sure you don't actually change your safe multithreaded code to make it unsafe is even harder.

One common pattern I see in almost every multithreaded codebase is something like such:

```
class Resources {
...
std::mutex mutex_;
Resource resource; // protected by mutex_
int i;  // protected by mutex_
...
}
```

This has two major issues:
1. Often developers don't bother commenting to show what objects are protected by mutexes and which are not, so it's difficult to know what exactly the critical section is.
2. It's very easy for someone using this code to not realize they need to lock the mutex if they didn't read the comment, or to otherwise accidentally use the critical section without making sure they have exclusive access.

There are tools to help avoid this. For example, [Clang Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html), but for code where that's not an option, I wrote this tool.

## TLDR:
Borrowing hides access to any content that needs to be secured by a mutex (the critical section), and only gives access to that content after it has secured the mutex, and then unlocks the mutex when that content goes out of scope. And it does so with very little overhead.

## Borrowed ownership is a simple, intuitive, and useful concept
Taking a page from the ownership model introduced with C++11 and smart pointers (Single-ownership for unique_ptr and shared ownership for shared_ptr), I introduce here the idea of borrowed ownership.

The idea of borrowed ownership is pretty simple. Imagine I live in a neighborhood with neighbors that often want to borrow my powertools. We will call that a `Borrowable<PowerTool> my_powertool(PowerTool(...))`.

I am always the owner of that powertool, but I may not have posession of it at any one time. In addition, I may have multiple friends that want to use it at the same time.

If one of my friends, let's call him thread1, wants to use my powertool, they will have to borrow it. That friend calls
`Borrowed<PowerTool> powertool = my_powertool.borrow();`

This grants thread1 exclusive access to the PowerTool for as long as he needs it. When `powertool` goes out of scope and its destructor is called, thread 1 releases its use of PowerTool automatically.

Let's say I have another friend, thread2, that also wants to borrow `my_powertool`, but thread1 is currently using it and modifying it. Thread2 calls `Borrowed<PowerTool> thread2_powertool = my_powertool.borrow()`. But since thread1 currently has access, thread2 will wait at this point until thread1 is finished with PowerTool.

It's just that simple!

## Comparison
```
struct CrticalResources {
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
Using borrowing:
```
class ThreadSafeResources {
  ...
  Borrowable<CriticalResources> resources_;

  void set_i(int i, const std::string& s) {
    Borrowed<CriticalResources> resources = resources_.borrow();
    resources->i = i;
    resources->printer.print(s);
  }
  ...
}
```

## It's got buzzwords!
- RAII: access to the critical section is revoked when the Borrowed goes out of scope, which is also when the mutex is unlocked.
- Smart pointers: The Borrowed object is a pointer-like object: use -> to access the members of the critical section and use * to dereference it to get the critical section!
- Synchronization: Access to the Borrowable object is thread-safe!

## Status

I have no intention of building this into a fully-featured suite. It has basic functionality and that's it. I'm open to pull requests if anyone wants to improve it!

Cool features to add would be:
- Allow the ability to use stack memory instead of always allocating on the heap using a unique_ptr internally(maybe using an std::variant<unique_ptr<T>, T> or template overloads of some kind?)
- Allow the ability to take the critical section object out of the Borrowable object
- Allow the ability to change the thing being borrowed (for example, to an updated object)
- Think about if it would be useful to allow different mutex types
- Add the ability to check if the resource is currently loaned out, instead of just trying to use is (the equivalent of try_lock)
- Add more programming languages
- Multi-level borrowing: Give neighbors the ability to loan out the powertool they loaned.
  
