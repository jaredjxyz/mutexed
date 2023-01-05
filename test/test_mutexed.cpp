#include <mutexed/mutexed.h>
#include <gtest/gtest.h>
#include <thread>

TEST(BorrowingTest, test_borrowing_twice) {
    mutexed::Mutexed<int> resource(4);
    mutexed::Owned<int> Owned = resource.own();
    EXPECT_EQ(*Owned, 4);
    // Trying to borrow from the same thread is undefined = launch it in another thread
    std::thread([&resource](){std::optional<mutexed::Owned<int>> i = resource.try_own(); EXPECT_EQ(i, std::nullopt);}).join();
}

TEST(BorrowingTest, raii_returning) {
    mutexed::Mutexed<int> i(4);
    {mutexed::Owned<int> Owned = i.own();}
    {mutexed::Owned<int> Owned = i.own();}
    EXPECT_EQ(*i.own(), 4);
}
