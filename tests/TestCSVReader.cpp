#include <gtest/gtest.h>
#include <crypto-platform/CSVReader.h>

TEST(CSVReaderTest, BasicAssertions) {
    CSVReader reader;

    {
        auto tokens { reader.tokenise("", ',')};
        EXPECT_EQ(tokens.size(), 0);
    }

    {
        auto tokens { reader.tokenise("test", ',')};
        EXPECT_EQ(tokens.size(), 1);
    }

    {
        auto tokens { reader.tokenise("test,", ',')};
        EXPECT_EQ(tokens.size(), 1);
    }

    {
        auto tokens { reader.tokenise("test1, test2", ',')};
        EXPECT_EQ(tokens.size(), 2);
        EXPECT_EQ(tokens[0], "test1");
        EXPECT_EQ(tokens[1], "test2");
    }
}