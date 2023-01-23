#pragma once

#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "completion-service.hpp"
#include "http-connector.hpp"

namespace Openai::Impl {

auto getApiKey() -> std::string
{
    const char* const ENV_VAR = "OPENAI_API_KEY";
    const char* const API_KEY = std::getenv(ENV_VAR);

    if (not API_KEY) {
        using namespace std::string_literals;
        throw std::invalid_argument{"Please provide your '"s + ENV_VAR + "' variable to use it's services."};
    }

    return API_KEY;
}

template <typename T>
auto MakeService() -> std::unique_ptr<T>
{
    using U = Impl::HttpConnector;
    static_assert(
        std::is_same_v<Impl::Completion::Completion, T>
    );

    auto connector = std::make_unique<U>("https://api.openai.com/v1", getApiKey());
    auto service   = std::make_unique<T>(std::move(connector));

    return service;
}

template <typename T, typename U>
auto MakeResult(std::unique_ptr<U>& service) -> std::unique_ptr<T>
{
    static_assert(
        (std::is_same_v<Impl::Completion::Result, T> and std::is_same_v<Impl::Completion::Completion, U>)
    );

    auto future = service->submitRequest();
    auto result = std::make_unique<T>(std::move(future));

    return result;
}

}

#define DEFINE_SPECIAL_MEMBERS_FOR_SERVICE(_wrapper_)                   \
                                                                        \
    _wrapper_::~_wrapper_() noexcept = default;                         \
    _wrapper_::_wrapper_(_wrapper_&&) noexcept = default;               \
    _wrapper_& _wrapper_::operator=(_wrapper_&&) noexcept = default;    \
                                                                        \
    static_assert(not std::is_copy_constructible_v<_wrapper_>);         \
    static_assert(not std::is_copy_assignable_v<_wrapper_>);            \
    static_assert(    std::is_move_constructible_v<_wrapper_>);         \
    static_assert(    std::is_move_assignable_v<_wrapper_>)

#define DEFINE_SPECIAL_MEMBERS_FOR_RESULT(_wrapper_)                                \
                                                                                    \
    _wrapper_::Result::Result() = default;                                          \
    _wrapper_::Result::~Result() noexcept = default;                                \
    _wrapper_::Result::Result(Result&&) noexcept = default;                         \
    _wrapper_::Result& _wrapper_::Result::operator=(Result &&) noexcept = default;  \
                                                                                    \
    static_assert(not std::is_copy_constructible_v<_wrapper_::Result>);             \
    static_assert(not std::is_copy_assignable_v<_wrapper_::Result>);                \
    static_assert(    std::is_move_constructible_v<_wrapper_::Result>);             \
    static_assert(    std::is_move_assignable_v<_wrapper_::Result>)
