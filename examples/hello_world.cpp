#include "borrowing/borrowing.h"

#include <iostream>
#include <thread>

int main() {
  using borrowing::Borrowable;
  using borrowing::Borrowed;

  // Turn the string into "Hello World"
  const std::string goal = "Hello world";

  Borrowable<std::string> str_owner("Hi");

  auto resize_string = [&](){
    Borrowed<std::string> str = str_owner.borrow();
    str->resize(goal.size());
    // Ownership is given back here, as it goes out of scope
  };

  auto make_helloworld = [&](){
    // Busy wait to find if string has been resized
    while (!(str_owner.borrow()->size() == goal.size())) {}
    *str_owner.borrow() = goal;
  };

  auto print_string = [&](){
    do {
      std::cout << *str_owner.cborrow() << std::endl;
    } while (*str_owner.borrow() != goal);
  };

  std::thread t3(print_string);
  std::thread t2(make_helloworld);
  std::thread t1(resize_string);

  t3.join();
  t2.join();
  t1.join();

  // str_owner.try_borrow(); // Works in C++17 and higher

  return 0;
}
