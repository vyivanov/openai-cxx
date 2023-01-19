#pragma once

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "rest-connector.hpp"

namespace Openai::Impl::Completion {

class Completion final {
public:
    explicit Completion(std::unique_ptr<RestConnector> connector) noexcept;

    void model(std::string&& id) noexcept;

    void prompt(std::string&& text) noexcept;
    void prompt(std::vector<std::string>&& text_list) noexcept;

    void temperature(float sampling) noexcept;

    void echo() noexcept;

    std::future<RestConnector::Response> submitRequest();
};

class Result final {
public:
    explicit Result(std::future<RestConnector::Response> response) noexcept;
};

}
