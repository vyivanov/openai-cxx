#include "fake-connector.hpp"

#include <cassert>
#include <future>
#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include "rest-connector.hpp"

namespace Openai::Impl {

FakeConnector::FakeConnector(Response to_be_returned): m_response(std::move(to_be_returned))
{
    assert(m_response.code);
    assert(not m_response.header.empty());
    assert(not m_response.body.empty());
}

auto FakeConnector::appendRoutePath(std::string&&) -> RestConnector&
{
    return (*this);
}

auto FakeConnector::addHeaderField(std::string&&, std::string&&) -> RestConnector&
{
    return (*this);
}

auto FakeConnector::setBodyContent(std::string&&) -> RestConnector&
{
    return (*this);
}

auto FakeConnector::submitGetRequest() -> std::future<Response>
{
    return make_response();
}

auto FakeConnector::submitPostRequest() -> std::future<Response>
{
    return make_response();
}

auto FakeConnector::make_response() -> std::future<Response>
{
    auto promise = std::promise<Response>{};
    auto future = promise.get_future();

    auto header_lower_case_keys = decltype(m_response.header){m_response.header.size()};
    BOOST_FOREACH(const auto& field, m_response.header) {
        [[maybe_unused]]
        const auto [_, ok] = header_lower_case_keys
            .emplace(std::pair{boost::to_lower_copy(field.first), field.second});
        assert(ok);
    }

    m_response.header = std::move(header_lower_case_keys);
    header_lower_case_keys = decltype(header_lower_case_keys){};

    promise.set_value(std::move(m_response));
    m_response = decltype(m_response){};

    return future;
}

}
