#include "completion.hpp"

#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "completion-service.hpp"
#include "make.hpp"

namespace Openai {

Completion::Completion(): m_pimpl(Impl::MakeService<Impl::Completion::Completion>())
{
    assert(m_pimpl);
}

auto Completion::model(std::string id) noexcept -> Completion&
{
    m_pimpl->model(std::move(id));

    return (*this);
}

auto Completion::prompt(std::string text) noexcept -> Completion&
{
    m_pimpl->prompt(std::move(text));

    return (*this);
}

auto Completion::prompt(std::vector<std::string> text_list) noexcept -> Completion&
{
    m_pimpl->prompt(std::move(text_list));

    return (*this);
}

auto Completion::temperature(float sampling) noexcept -> Completion&
{
    m_pimpl->temperature(sampling);

    return (*this);
}

auto Completion::echo() noexcept -> Completion&
{
    m_pimpl->echo();

    return (*this);
}

auto Completion::create() -> Result
{
    auto result = Completion::Result{};
    result.m_pimpl = Impl::MakeResult<Impl::Completion::Result>(m_pimpl);

    assert(result.m_pimpl);
    return result;
}

DEFINE_SPECIAL_MEMBERS_FOR_SERVICE(Completion);
DEFINE_SPECIAL_MEMBERS_FOR_RESULT(Completion);

}
