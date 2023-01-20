#pragma once

#include <future>
#include <optional>
#include <string>

#include "rest-connector.hpp"

namespace Openai::Impl {

struct HttpConnector final: RestConnector {
    explicit HttpConnector(std::string&& url, std::optional<std::string>&& bearer={});

    RestConnector& appendRoutePath(std::string&& value) override;
    RestConnector& addHeaderField(std::string&& name, std::string&& value) override;
    RestConnector& setBodyContent(std::string&& data) override;

    std::future<Response> submitGetRequest() override;
    std::future<Response> submitPostRequest() override;
};

}
