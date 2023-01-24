#include <gtest/gtest.h>

#include "rest-connector.hpp"

using Openai::Impl::RestConnector;

TEST(response_comparator, 1)
{
    auto a = RestConnector::Response{};
    auto b = RestConnector::Response{};

    EXPECT_TRUE(a == b);
}

TEST(response_comparator, 2)
{
    auto a = RestConnector::Response{};
    auto b = RestConnector::Response{
        .code   = {},
        .header = {},
        .body   = "12",
    };

    EXPECT_TRUE(a != b);
}

TEST(response_comparator, 3)
{
    auto a = RestConnector::Response{};
    auto b = RestConnector::Response{
        .code   = {},
        .header = {{"1", "2"}},
        .body   = {},
    };

    EXPECT_TRUE(a != b);
}

TEST(response_comparator, 4)
{
    auto a = RestConnector::Response{};
    auto b = RestConnector::Response{
        .code   = 12,
        .header = {},
        .body   = {},
    };

    EXPECT_TRUE(a != b);
}

TEST(response_comparator, 5)
{
    auto a = RestConnector::Response{};
    auto b = RestConnector::Response{
        .code   = {},
        .header = {{"1", "2"}},
        .body   = "12",
    };

    EXPECT_TRUE(a != b);
}

TEST(response_comparator, 6)
{
    auto a = RestConnector::Response{};
    auto b = RestConnector::Response{
        .code   = 12,
        .header = {},
        .body   = "12",
    };

    EXPECT_TRUE(a != b);
}

TEST(response_comparator, 7)
{
    auto a = RestConnector::Response{};
    auto b = RestConnector::Response{
        .code   = 12,
        .header = {{"1", "2"}},
        .body   = {},
    };

    EXPECT_TRUE(a != b);
}
