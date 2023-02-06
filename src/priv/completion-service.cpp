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

Completion::Completion(std::unique_ptr<RestConnector> connector) noexcept
    : m_connector(std::move(connector))
{
    assert(m_connector);

    m_connector->appendRoutePath(SERVICE_ROUTE_PATH);
    m_connector->addHeaderField("content-type", "application/json");

    assert(not m_model);
    assert(not m_prompt);
    assert(not m_temperature);
    assert(not m_echo);
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

Result::Result(std::future<RestConnector::Response> response) noexcept
    : m_response(std::move(response))
{
    assert(m_response.valid());
    assert(m_id.empty());
    assert(m_created == decltype(m_created){});
}

auto Result::id() noexcept -> std::string
{
    parse();
    return m_id;
}

auto Result::created() noexcept -> std::time_t
{
    parse();
    return m_created;
}

void Result::parse()
{
    if (not m_response.valid()) {
        return;
    }

    auto response = m_response.get();
    auto body = nlohmann::json::parse(std::move(response.body));

    (void) response.code;
    CALL_FROM_JSON(Result, this, std::move(body));
}

DEFINE_FROM_JSON(Result, id, created);

}
