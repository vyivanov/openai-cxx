#pragma once

#include <future>
#include <string>

#include "rest-connector.hpp"

namespace Openai::Impl {

struct FakeConnector final: RestConnector {
    FakeConnector() = default;
    explicit FakeConnector(Response to_be_returned);

    RestConnector& appendRoutePath(std::string&& value) override;
    RestConnector& addHeaderField(std::string&& name, std::string&& value) override;
    RestConnector& setBodyContent(std::string&& data) override;

    std::future<Response> submitGetRequest() override;
    std::future<Response> submitPostRequest() override;

private:
    std::future<Response> make_response();
    Response m_response = {};
};

}
