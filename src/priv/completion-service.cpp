#include "completion-service.hpp"

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "rest-connector.hpp"

namespace Openai::Impl::Completion {

Completion::Completion(std::unique_ptr<RestConnector> connector) noexcept
{
    (void) connector;
}

void Completion::model(std::string&& id) noexcept
{
    (void) id;
}

void Completion::prompt(std::string&& text) noexcept
{
    (void) text;
}

void Completion::prompt(std::vector<std::string>&& text_list) noexcept
{
    (void) text_list;
}

void Completion::temperature(float sampling) noexcept
{
    (void) sampling;
}

void Completion::echo() noexcept
{

}

auto Completion::submitRequest() -> std::future<RestConnector::Response>
{
    return decltype(submitRequest()){};
}

Result::Result(std::future<RestConnector::Response> response) noexcept
{
    (void) response;
}

}
