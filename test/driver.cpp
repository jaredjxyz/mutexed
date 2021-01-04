#include "borrowing/borrowing.h"

int main() {
  borrowing::Borrowable<int> i(std::make_unique<int>(1));
  return 0;
}
