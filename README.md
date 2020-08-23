## Example

```
#include "borrowing/borrowing.h"

using namespace borrowing;


struct X {
  int i;
  X(int i) : i(i) {}
  X(const X& other) = default;
  X& operator=(const X& other) {
    i = other.i;
    return *this;
  }
};

int main() {
  Borrowable<X> borrowable(2);
  std::thread t1([&borrowable]() {
    X x(4);
    Borrowed<X> b = borrowable.borrow();
    *b = x;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Hi" << std::endl;
  });

  std::thread t2([&borrowable]() {
    std::cout << "Starting 2" << std::endl;
    Borrowed<X> x = borrowable.borrow();
    std::cout << "Got 2" << std::endl;
  });

  t1.join();
  t2.join();
  return 0;
}
```
