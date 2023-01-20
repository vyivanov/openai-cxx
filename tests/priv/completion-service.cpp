#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "completion-service.hpp"
#include "mock-connector.hpp"

namespace Impl = Openai::Impl;
using namespace std::string_literals;

void setCallExpectations(
    const std::unique_ptr<Impl::MockConnector>& for_mock,
    const nlohmann::json& expected_body)
{
    auto route = std::string{Impl::Completion::Completion::SERVICE_ROUTE_PATH};

    EXPECT_CALL((*for_mock), appendRoutePath(std::move(route))).Times(1);
    EXPECT_CALL((*for_mock), addHeaderField("content-type"s, "application/json"s)).Times(1);
    EXPECT_CALL((*for_mock), setBodyContent(expected_body.dump())).Times(1);
    EXPECT_CALL((*for_mock), submitGetRequest()).Times(0);
    EXPECT_CALL((*for_mock), submitPostRequest()).Times(1);
}

TEST(connector_input, completion_without_parameters)
{
    auto mock = std::make_unique<Impl::MockConnector>();
    ::setCallExpectations(mock, nlohmann::json{});

    auto response = Impl::Completion::Completion{std::move(mock)}.submitRequest().get();
    EXPECT_EQ(response, decltype(response){});
}

TEST(connector_input, completion_with_model_parameter)
{
    auto model_id = "model_id"s;
    auto expected_body = nlohmann::json{};
    expected_body["model"] = model_id;

    auto mock = std::make_unique<Impl::MockConnector>();
    ::setCallExpectations(mock, expected_body);

    auto completion = Impl::Completion::Completion{std::move(mock)};
    completion.model(std::move(model_id));

    auto response = completion.submitRequest().get();
    EXPECT_EQ(response, decltype(response){});
}

TEST(connector_input, completion_with_single_prompt_parameter)
{
    auto prompt = "prompt"s;
    auto expected_body = nlohmann::json{};
    expected_body["prompt"] = prompt;

    auto mock = std::make_unique<Impl::MockConnector>();
    ::setCallExpectations(mock, expected_body);

    auto completion = Impl::Completion::Completion{std::move(mock)};
    completion.prompt(std::move(prompt));

    auto response = completion.submitRequest().get();
    EXPECT_EQ(response, decltype(response){});
}

TEST(connector_input, completion_with_vector_prompt_parameter)
{
    auto prompts = std::vector<std::string>{"prompt_0", "prompt_1"};
    auto expected_body = nlohmann::json{};
    expected_body["prompt"] = prompts;

    auto mock = std::make_unique<Impl::MockConnector>();
    ::setCallExpectations(mock, expected_body);

    auto completion = Impl::Completion::Completion{std::move(mock)};
    completion.prompt(std::move(prompts));

    auto response = completion.submitRequest().get();
    EXPECT_EQ(response, decltype(response){});
}

TEST(connector_input, completion_with_temperature_parameter)
{
    auto temperature = 0.4242f;
    auto expected_body = nlohmann::json{};
    expected_body["temperature"] = temperature;

    auto mock = std::make_unique<Impl::MockConnector>();
    ::setCallExpectations(mock, expected_body);

    auto completion = Impl::Completion::Completion{std::move(mock)};
    completion.temperature(temperature);

    auto response = completion.submitRequest().get();
    EXPECT_EQ(response, decltype(response){});
}

TEST(connector_input, completion_with_echo_parameter)
{
    auto expected_body = nlohmann::json{};
    expected_body["echo"] = true;

    auto mock = std::make_unique<Impl::MockConnector>();
    ::setCallExpectations(mock, expected_body);

    auto completion = Impl::Completion::Completion{std::move(mock)};
    completion.echo();

    auto response = completion.submitRequest().get();
    EXPECT_EQ(response, decltype(response){});
}
