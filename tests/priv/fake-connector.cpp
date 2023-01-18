#include <array>
#include <functional>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <gtest/gtest.h>

#include "fake-connector.hpp"

using Openai::Impl::FakeConnector;
using namespace std::string_literals;

constexpr auto SUBMIT_REQUEST_METHODS = std::array {
    &FakeConnector::submitGetRequest,
    &FakeConnector::submitPostRequest,
};

template <typename... Ts>
void iterateOverSubmitRequests(
    const std::function<void(std::function<FakeConnector::Response()>)>& validate,
    Ts&&... args)
{
    BOOST_FOREACH(const auto pfn, ::SUBMIT_REQUEST_METHODS) {
        auto fake = FakeConnector(std::forward<Ts>(args)...);
        validate([&] {
            return std::bind(pfn, &fake)().get();
        });
    }
}

TEST(test_params_throughing, pass_empty_response_fields)
{
    ::iterateOverSubmitRequests(
        [] (const std::function<FakeConnector::Response()>& submitRequest) {
            const auto resp = submitRequest();

            EXPECT_EQ(resp.code, decltype(resp.code){});
            EXPECT_EQ(resp.header, decltype(resp.header){});
            EXPECT_EQ(resp.body, decltype(resp.body){});
        }
    );
}

TEST(test_params_throughing, pass_header_and_body)
{
    const auto code = 200;
    const auto header = FakeConnector::Response::Header{{"A","A"}, {"b","b"}};
    const auto body = R"json({"A":"A", "b":"b"})json"s;

    auto header_lower_case_keys = decltype(header){header.size()};
    BOOST_FOREACH(const auto& field, header) {
        [[maybe_unused]]
        const auto [_, ok] = header_lower_case_keys
            .emplace(std::pair{boost::to_lower_copy(field.first), field.second});
        EXPECT_TRUE(ok);
    }

    ::iterateOverSubmitRequests(
        [&] (const std::function<FakeConnector::Response()>& submitRequest) {
            const auto resp = submitRequest();

            EXPECT_EQ(resp.code, code);
            EXPECT_EQ(resp.header, header_lower_case_keys);
            EXPECT_EQ(resp.body, body);
        },
        code, header, body
    );
}

TEST(test_params_invalidation, double_call_to_submition)
{
    ::iterateOverSubmitRequests(
        [] (const std::function<FakeConnector::Response()>& submitRequest) {
            [[maybe_unused]]
            const auto resp_1 = submitRequest();
            const auto resp_2 = submitRequest();

            EXPECT_EQ(resp_2.code, decltype(resp_2.code){});
            EXPECT_EQ(resp_2.header, decltype(resp_2.header){});
            EXPECT_EQ(resp_2.body, decltype(resp_2.body){});
        },
        FakeConnector::Response::Code{1},
        FakeConnector::Response::Header{{"2", "3"}},
        FakeConnector::Response::Body{"4"}
    );
}
