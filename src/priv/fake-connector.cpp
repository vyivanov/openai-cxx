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

RestConnector& FakeConnector::appendRoutePath(std::string&&)
{
    return (*this);
}

RestConnector& FakeConnector::addHeaderField(std::string&&, std::string&&)
{
    return (*this);
}

RestConnector& FakeConnector::setBodyContent(std::string&&)
{
    return (*this);
}

std::future<RestConnector::Response> FakeConnector::submitGetRequest()
{
    return make_response();
}

std::future<RestConnector::Response> FakeConnector::submitPostRequest()
{
    return make_response();
}

std::future<RestConnector::Response> FakeConnector::make_response()
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
