#include <gtest/gtest.h>
#include <crypto-platform/MatchingSystem.h>

TEST(MatchSystemTest, BasicAssertions) {
    MatchSystem match;

    {
        // Correct amount of products to load in
        int count = match.readCSV_NEW("/Users/james/Projects/C++ Crypto Trading/testing_suite/crypto_platform/20200317.csv");

        // We expcect count to be 3450
        EXPECT_EQ(count, 3540);
    }

    {
        // Large order, so there will be a match
        auto matches_1 = match.testMatch(false);

        // We would expcet to see no matches here
        EXPECT_EQ(matches_1.size(), 0);
    }

    {
        // Small order, so there will be no match
        auto matches_2 = match.testMatch(true);

        // We would expect to see 1 match here
        EXPECT_EQ(matches_2.size(), 1);
    }
}