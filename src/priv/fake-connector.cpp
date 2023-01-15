#include "fake-connector.hpp"

#include <cassert>
#include <future>
#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

namespace Openai::Impl {

FakeConnector::FakeConnector(Response::Code code, Response::Header header, Response::Body body)
    : m_code(code)
    , m_header(std::move(header))
    , m_body(std::move(body))
{

}

FakeConnector& FakeConnector::appendRoutePath(std::string&&)
{
    return (*this);
}

FakeConnector& FakeConnector::addHeaderField(std::string&&, std::string&&)
{
    return (*this);
}

FakeConnector& FakeConnector::setBodyContent(std::string&&)
{
    return (*this);
}

std::future<FakeConnector::Response> FakeConnector::submitGetRequest()
{
    return make_response();
}

std::future<FakeConnector::Response> FakeConnector::submitPostRequest()
{
    return make_response();
}

std::future<FakeConnector::Response> FakeConnector::make_response()
{
    auto promise = std::promise<FakeConnector::Response>{};
    auto future = promise.get_future();

    auto header_lower_case_keys = decltype(m_header){m_header.size()};
    BOOST_FOREACH(const auto& field, m_header) {
        [[maybe_unused]]
        const auto [_, ok] = header_lower_case_keys
            .emplace(std::pair{boost::to_lower_copy(field.first), field.second});
        assert(ok);
    }

    promise.set_value({
        .code = m_code,
        .header = std::move(header_lower_case_keys),
        .body = std::move(m_body),
    });

    m_code = decltype(m_code){};
    m_header = decltype(m_header){};
    m_body = decltype(m_body){};

    return future;
}

}
