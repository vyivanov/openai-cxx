#pragma once

#include <future>
#include <string>

#include "rest-connector.hpp"

namespace Openai::Impl {

struct FakeConnector final: RestConnector {
    FakeConnector(Response::Code code, Response::Header header, Response::Body body);

    FakeConnector& appendRoutePath(std::string&& value) override;
    FakeConnector& addHeaderField(std::string&& name, std::string&& value) override;
    FakeConnector& setBodyContent(std::string&& data) override;

    std::future<Response> submitGetRequest() override;
    std::future<Response> submitPostRequest() override;
};

}
