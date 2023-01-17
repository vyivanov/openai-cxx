#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "fake-connector.hpp"
#include "mock-connector.hpp"
#include "rest-connector.hpp"

using Openai::Impl::FakeConnector;
using Openai::Impl::MockConnector;
using Openai::Impl::RestConnector;

TEST(carrying, default_ctor)
{
    using ::testing::_;
    using namespace std::string_literals;

    auto mock = MockConnector{};

    EXPECT_CALL(mock, appendRoutePath(_)).Times(6);
    EXPECT_CALL(mock, addHeaderField(_, _)).Times(6);
    EXPECT_CALL(mock, setBodyContent(_)).Times(6);

    mock.appendRoutePath(""s).addHeaderField(""s, ""s).setBodyContent(""s);
    mock.addHeaderField(""s, ""s).appendRoutePath(""s).setBodyContent(""s);
    mock.addHeaderField(""s, ""s).setBodyContent(""s).appendRoutePath(""s);
    mock.setBodyContent(""s).addHeaderField(""s, ""s).appendRoutePath(""s);
    mock.setBodyContent(""s).appendRoutePath(""s).addHeaderField(""s, ""s);
    mock.appendRoutePath(""s).setBodyContent(""s).addHeaderField(""s, ""s);
}

TEST(delegation, default_ctor)
{
    auto mock = MockConnector{};

    EXPECT_CALL(mock, submitGetRequest).Times(1);
    auto resp = mock.submitGetRequest().get();

    EXPECT_EQ(resp.code, decltype(resp.code){});
    EXPECT_EQ(resp.header, decltype(resp.header){});
    EXPECT_EQ(resp.body, decltype(resp.body){});
}

TEST(delegation, non_default_ctor)
{
    const auto code = RestConnector::Response::Code{100};
    const auto header = RestConnector::Response::Header{{"200", "300"}};
    const auto body = RestConnector::Response::Body{"400"};

    auto fake = FakeConnector{{code, header, body}};
    auto mock = MockConnector{std::move(fake)};

    EXPECT_CALL(mock, submitGetRequest).Times(1);
    auto resp = mock.submitGetRequest().get();

    EXPECT_EQ(resp.code, code);
    EXPECT_EQ(resp.header, header);
    EXPECT_EQ(resp.body, body);
}
