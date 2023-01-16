#include "fake-connector.hpp"

#include <future>
#include <string>

namespace Openai::Impl {

FakeConnector::FakeConnector(Response::Code code, Response::Header header, Response::Body body)
{
    (void) code; (void) header; (void) body;
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
    return std::future<FakeConnector::Response>{};
}

std::future<FakeConnector::Response> FakeConnector::submitPostRequest()
{
    return std::future<FakeConnector::Response>{};
}

}
