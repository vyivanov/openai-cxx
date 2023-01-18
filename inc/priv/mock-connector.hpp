#pragma once

#include <future>
#include <string>

#include <gmock/gmock.h>

#include "fake-connector.hpp"
#include "rest-connector.hpp"

namespace Openai::Impl {

struct MockConnector final: RestConnector {
    MockConnector() noexcept;
    explicit MockConnector(FakeConnector fake) noexcept;

    MOCK_METHOD((RestConnector&), appendRoutePath, (std::string&&), (override));
    MOCK_METHOD((RestConnector&), addHeaderField, (std::string&&, std::string&&), (override));
    MOCK_METHOD((RestConnector&), setBodyContent, (std::string&&), (override));

    MOCK_METHOD((std::future<Response>), submitGetRequest, (), (override));
    MOCK_METHOD((std::future<Response>), submitPostRequest, (), (override));

private:
    void allowDelegationToFake() noexcept;
    FakeConnector m_fake = {};
};

}
