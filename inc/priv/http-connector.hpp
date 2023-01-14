#pragma once

#include <future>
#include <memory>
#include <optional>
#include <string>

#include <cpr/cpr.h>

#include "rest-connector.hpp"

namespace Openai::Impl {

struct HttpConnector final: RestConnector {
    explicit HttpConnector(std::string&& url, std::optional<std::string>&& bearer={});

    RestConnector& appendRoutePath(std::string&& value) override;
    RestConnector& addHeaderField(std::string&& name, std::string&& value) override;
    RestConnector& setBodyContent(std::string&& data) override;

    std::future<Response> submitGetRequest() override;
    std::future<Response> submitPostRequest() override;

private:
    std::shared_ptr<cpr::Session> make_session();

    cpr::Url m_url;
    cpr::Header m_header;
    cpr::Body m_body;
};

}
