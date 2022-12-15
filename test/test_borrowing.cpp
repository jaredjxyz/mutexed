#include <borrowing/borrowing.h>
#include <gtest/gtest.h>
#include <thread>

TEST(BorrowingTest, test_borrowing_twice) {
    borrowing::Borrowable<int> resource(4);
    borrowing::Borrowed<int> borrowed = resource.borrow();
    EXPECT_EQ(*borrowed, 4);
    // Trying to borrow from the same thread is undefined = launch it in another thread
    std::thread([&resource](){std::optional<borrowing::Borrowable<int>> i = resource.try_borrow(); EXPECT_EQ(i, std::nullopt);}).join();
}

TEST(BorrowingTest, raii_returning) {
    borrowing::Borrowable<int> i(4);
    {borrowing::Borrowed<int> borrowed = i.borrow();}
    {borrowing::Borrowed<int> borrowed = i.borrow();}
    EXPECT_EQ(*i.borrow(), 4);
}
