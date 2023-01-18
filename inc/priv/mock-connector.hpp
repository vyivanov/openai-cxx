#pragma once

#include <future>
#include <string>

#include <gmock/gmock.h>

#include "rest-connector.hpp"

namespace Openai::Impl {

struct MockConnector final: RestConnector {
    MOCK_METHOD((RestConnector&), appendRoutePath, (std::string&&), (override));
    MOCK_METHOD((RestConnector&), addHeaderField, (std::string&&, std::string&&), (override));
    MOCK_METHOD((RestConnector&), setBodyContent, (std::string&&), (override));

    MOCK_METHOD((std::future<Response>), submitGetRequest, (), (override));
    MOCK_METHOD((std::future<Response>), submitPostRequest, (), (override));
};

}
