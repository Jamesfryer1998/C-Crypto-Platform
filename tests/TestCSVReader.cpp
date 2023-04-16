#include <gtest/gtest.h>
#include "crypto-platform/CSVReader.h"

TEST(CSVReaderTest, BasicAssertions) {
    CSVReader reader;

    {
        // Given an empty line
        // When we tokenize it
        auto tokens { reader.tokenise("", ',')};
        // Then we expect the result to be no tokens
        EXPECT_EQ(tokens.size(), 0);
    }

    {
        // Given a line with a single world
        // When we tokenize it
        auto tokens { reader.tokenise("test", ',')};
        // Then we expect the word as the only token
        EXPECT_EQ(tokens.size(), 1);
        EXPECT_EQ(tokens[0], "test");
    }

    {
        // Given a line with a single word and a trailing comma
        // When we tokenize it
        auto tokens { reader.tokenise("test,", ',')};
        EXPECT_EQ(tokens.size(), 1);
        // Then we expect the word as the only token
        EXPECT_EQ(tokens[0], "test");
    }

    {
        // Given a line with a single word and a trailing command
        // When we tokenize it
        auto tokens { reader.tokenise("test1,,test2", ',')};
        // Then we expect what????, presumably more than one token. This test
        // fails
        EXPECT_GE(tokens.size(), 2);
    }

    {
        // Given a line with two words and a leading space 
        // When we tokenize it
        auto tokens { reader.tokenise("test1, test2", ',')};
        // Then we expect two words including the leading space
        EXPECT_EQ(tokens.size(), 2);
        EXPECT_EQ(tokens[0], "test1");
        EXPECT_EQ(tokens[1], " test2");
    }
}