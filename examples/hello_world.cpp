#include "borrowing/borrowing.h"

#include <chrono>
#include <iostream>
#include <thread>

int main() {
  using borrowing::Borrowable;
  using borrowing::Borrowed;

  // Turn the string into "Hello World"
  const std::string goal = "Hello world";

  Borrowable<std::string> str_owner("Hi");

  auto change_string = [&](const std::string& s){
    auto init_time = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - init_time < std::chrono::seconds(1)) {
      Borrowed<std::string> str = str_owner.borrow();
      *str = s;
    }

    str_owner.borrow()->clear();
  };

  auto print_string = [&](){
    do {
      std::cout << *str_owner.cborrow() << std::endl;
    } while (!str_owner.cborrow()->empty());
  };

  // Should print "Hi", "Hello World", or "Hi mom", but not a jumbled mess
  // of characters that would happen in a non thread-safe environment
  std::thread t3(print_string);
  std::thread t2(change_string, "Hello World!");
  std::thread t1(change_string, "Hi mom!");

  t3.join();
  t2.join();
  t1.join();

  return 0;
}
