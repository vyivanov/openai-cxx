#pragma once

#include <future>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "rest-connector.hpp"
#include "ser-des.hpp"

namespace Openai::Impl::Completion {

class Completion final {
public:
    constexpr static auto SERVICE_ROUTE_PATH = "/completions";

    explicit Completion(std::unique_ptr<RestConnector> connector) noexcept;

    void model(std::string&& id) noexcept;

    void prompt(std::string&& text) noexcept;
    void prompt(std::vector<std::string>&& text_list) noexcept;

    void temperature(float sampling) noexcept;

    void echo() noexcept;

    std::future<RestConnector::Response> submitRequest();

private:
    std::unique_ptr<RestConnector> m_connector = {};

    std::optional<std::string> m_model = {};
    std::optional<std::variant<std::string, std::vector<std::string>>> m_prompt = {};
    std::optional<float> m_temperature = {};
    std::optional<bool> m_echo = {};

    DECLARE_TO_JSON(Completion);
};

class Result final {
public:
    explicit Result(std::future<RestConnector::Response> response) noexcept;
private:
    std::future<RestConnector::Response> m_response = {};
};

}
