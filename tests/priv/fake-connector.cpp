#include <array>
#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <gtest/gtest.h>

#include "fake-connector.hpp"
#include "rest-connector.hpp"

using Openai::Impl::FakeConnector;
using Openai::Impl::RestConnector;

using namespace std::string_literals;

constexpr auto SUBMIT_REQUEST_METHODS = std::array {
    std::mem_fn(&FakeConnector::submitGetRequest),
    std::mem_fn(&FakeConnector::submitPostRequest),
};

template <typename... Ts>
void iterateOverSubmitRequests(
    const std::function<void(std::function<RestConnector::Response()>)>& validate, const Ts&... args)
{
    BOOST_FOREACH(const auto& pfn, ::SUBMIT_REQUEST_METHODS) {
        auto fake = FakeConnector(args...);
        validate([&] {
            return pfn(fake).get();
        });
    }
}

TEST(test_params_throughing, pass_empty_response_fields)
{
    ::iterateOverSubmitRequests(
        [] (const auto& submitRequest) {
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
    const auto header = RestConnector::Response::Header{{"A","A"}, {"b","b"}};
    const auto body = R"json({"A":"A", "b":"b"})json"s;

    auto header_lower_case_keys = decltype(header){header.size()};
    BOOST_FOREACH(const auto& field, header) {
        [[maybe_unused]]
        const auto [_, ok] = header_lower_case_keys
            .emplace(std::pair{boost::to_lower_copy(field.first), field.second});
        EXPECT_TRUE(ok);
    }

    ::iterateOverSubmitRequests(
        [&] (const auto& submitRequest) {
            const auto resp = submitRequest();

            EXPECT_EQ(resp.code, code);
            EXPECT_EQ(resp.header, header_lower_case_keys);
            EXPECT_EQ(resp.body, body);
        },
        RestConnector::Response {
            .code = code,
            .header = header,
            .body = body,
        }
    );
}

TEST(test_params_invalidation, double_call_to_submition)
{
    ::iterateOverSubmitRequests(
        [] (const auto& submitRequest) {
            [[maybe_unused]]
            const auto resp_1 = submitRequest();
            const auto resp_2 = submitRequest();

            EXPECT_EQ(resp_2.code, decltype(resp_2.code){});
            EXPECT_EQ(resp_2.header, decltype(resp_2.header){});
            EXPECT_EQ(resp_2.body, decltype(resp_2.body){});
        },
        RestConnector::Response {
            .code = 1,
            .header = {{"2", "3"}},
            .body = "4",
        }
    );
}
