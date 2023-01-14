#include "http-connector.hpp"

#include <cassert>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <cpr/cpr.h>

namespace {
using namespace Openai::Impl;

auto convert(std::future<cpr::Response>) -> std::future<RestConnector::Response>;

}

namespace Openai::Impl {
using namespace std::string_literals;

HttpConnector::HttpConnector(std::string&& url, std::optional<std::string>&& bearer)
    : m_url(std::move(url))
{
    assert(m_url.str().length() > 0);
    assert(m_url.str().back() != '/');

    assert(m_header.empty());
    assert(m_body.str().empty());

    if (bearer.has_value()) {
        addHeaderField("Authorization", "Bearer "s + (*bearer));
    }
}

auto HttpConnector::appendRoutePath(std::string&& value) -> RestConnector&
{
    assert(not value.empty());
    assert(value.front() == '/');
    assert(value.back() != '/');

    m_url = m_url.str() + std::move(value);
    return (*this);
}

auto HttpConnector::addHeaderField(std::string&& name, std::string&& value) -> RestConnector&
{
    assert(not name.empty());
    assert(not value.empty());

    [[maybe_unused]]
    const auto [_, ok] = m_header.emplace(
        std::pair{std::move(name), std::move(value)}
    );

    assert(ok);
    return (*this);
}

auto HttpConnector::setBodyContent(std::string&& data) -> RestConnector&
{
    assert(not data.empty());

    m_body = std::move(data);
    return (*this);
}

auto HttpConnector::submitGetRequest() -> std::future<Response>
{
    return ::convert(make_session()->GetAsync());
}

auto HttpConnector::submitPostRequest() -> std::future<Response>
{
    return ::convert(make_session()->PostAsync());
}

auto HttpConnector::make_session() -> std::shared_ptr<cpr::Session>
{
    auto session = std::make_shared<cpr::Session>();

    session->SetOption(m_url);
    session->SetOption(m_header);

    session->SetOption(std::move(m_body));
    m_body = decltype(m_body){};

    return session;
}

}

namespace {

auto convert(std::future<cpr::Response> future) -> std::future<RestConnector::Response>
{
    assert(future.valid());
    return cpr::async(
        [](auto& future) -> RestConnector::Response {
            auto response = future.get();
            if (response.error) {
                throw std::runtime_error{response.error.message};
            }
            auto to_hashmap = [](cpr::Header&& header) {
                auto hashmap = RestConnector::Response::Header{header.size()};
                for (auto&& [key, val]: header) {
                    [[maybe_unused]]
                    const auto [_, ok] = hashmap.emplace(
                        std::pair{boost::to_lower_copy(key), std::move(val)});
                    assert(ok);
                }
                header = decltype(header){};
                return hashmap;
            };
            return {
                .code = static_cast<decltype(RestConnector::Response::code)>(response.status_code),
                .header = to_hashmap(std::move(response.header)),
                .body = std::move(response.text),
            };
        },
        std::move(future)
    );
}

}
