#pragma once

#include <future>
#include <string>
#include <unordered_map>

namespace Openai::Impl {

struct RestConnector {
    struct Response {
        using Code = std::uint16_t;
        using Header = std::unordered_map<std::string, std::string>;
        using Body = std::string;

        Code code;
        Header header;
        Body body;
    };

    virtual RestConnector& appendRoutePath(std::string&& value) = 0;
    virtual RestConnector& addHeaderField(std::string&& name, std::string&& value) = 0;
    virtual RestConnector& setBodyContent(std::string&& data) = 0;

    virtual std::future<Response> submitGetRequest() = 0;
    virtual std::future<Response> submitPostRequest() = 0;

    virtual ~RestConnector() = default;
};

}
