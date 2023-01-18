#include "mock-connector.hpp"

#include <string>
#include <utility>

#include <gmock/gmock.h>

#include "fake-connector.hpp"
#include "rest-connector.hpp"

namespace Openai::Impl {

MockConnector::MockConnector() noexcept: m_fake{}
{
    allowDelegationToFake();
}

MockConnector::MockConnector(FakeConnector fake) noexcept: MockConnector()
{
    m_fake = std::move(fake);
}

void MockConnector::allowDelegationToFake() noexcept
{
    ON_CALL((*this), appendRoutePath)
        .WillByDefault(
            [this] (std::string&& value) -> RestConnector& {
                m_fake.appendRoutePath(std::move(value));
                return (*this);
            }
        );

    ON_CALL((*this), addHeaderField)
        .WillByDefault(
            [this] (std::string&& name, std::string&& value) -> RestConnector& {
                m_fake.addHeaderField(std::move(name), std::move(value));
                return (*this);
            }
        );

    ON_CALL((*this), setBodyContent)
        .WillByDefault(
            [this] (std::string&& content) -> RestConnector& {
                m_fake.setBodyContent(std::move(content));
                return (*this);
            }
        );

    ON_CALL((*this), submitGetRequest)
        .WillByDefault(
            [this] {
                return m_fake.submitGetRequest();
            }
        );

    ON_CALL((*this), submitPostRequest)
        .WillByDefault(
            [this] {
                return m_fake.submitPostRequest();
            }
        );
}

}
