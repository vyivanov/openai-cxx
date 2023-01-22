#include "completion-service.hpp"

#include <cassert>
#include <future>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "rest-connector.hpp"
#include "ser-des.hpp"

namespace Openai::Impl::Completion {

Completion::Completion(std::unique_ptr<RestConnector> connector) noexcept:
    m_connector(std::move(connector))
{
    assert(m_connector);

    m_connector->appendRoutePath(SERVICE_ROUTE_PATH);
    m_connector->addHeaderField("content-type", "application/json");
}

void Completion::model(std::string&& id) noexcept
{
    m_model = std::move(id);
}

void Completion::prompt(std::string&& text) noexcept
{
    m_prompt = std::move(text);
}

void Completion::prompt(std::vector<std::string>&& text_list) noexcept
{
    m_prompt = std::move(text_list);
}

void Completion::temperature(float sampling) noexcept
{
    m_temperature = sampling;
}

void Completion::echo() noexcept
{
    m_echo = true;
}

auto Completion::submitRequest() -> std::future<RestConnector::Response>
{
    m_connector->setBodyContent(CALL_TO_JSON(Completion, this).dump());
    return m_connector->submitPostRequest();
}

DEFINE_TO_JSON(Completion, model, prompt, temperature, echo);

Result::Result(std::future<RestConnector::Response> response) noexcept:
    m_response(std::move(response))
{
    assert(m_response.valid());
}

}
