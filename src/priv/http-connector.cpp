#include "http-connector.hpp"

#include <future>
#include <optional>
#include <string>

namespace Openai::Impl {

HttpConnector::HttpConnector(std::string&& url, std::optional<std::string>&& bearer)
{
    (void) url; (void) bearer;
}

auto HttpConnector::appendRoutePath(std::string&& value) -> RestConnector&
{
    (void) value; return (*this);
}

auto HttpConnector::addHeaderField(std::string&& name, std::string&& value) -> RestConnector&
{
    (void) name; (void) value; return (*this);
}

auto HttpConnector::setBodyContent(std::string&& data) -> RestConnector&
{
    (void) data; return (*this);
}

auto HttpConnector::submitGetRequest() -> std::future<Response>
{
    return std::future<Response>{};
}

auto HttpConnector::submitPostRequest() -> std::future<Response>
{
    return std::future<Response>{};
}

}
