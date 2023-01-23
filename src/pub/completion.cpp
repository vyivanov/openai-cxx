#include "completion.hpp"

#include <string>
#include <vector>

namespace Openai {

Completion::Completion()
{

}

auto Completion::model(std::string id) noexcept -> Completion&
{
    (void) id; return (*this);
}

auto Completion::prompt(std::string text) noexcept -> Completion&
{
    (void) text; return (*this);
}

auto Completion::prompt(std::vector<std::string> text_list) noexcept -> Completion&
{
    (void) text_list; return (*this);
}

auto Completion::temperature(float sampling) noexcept -> Completion&
{
    (void) sampling; return (*this);
}

auto Completion::echo() noexcept -> Completion&
{
    return (*this);
}

auto Completion::create() -> Result
{
    return Completion::Result{};
}

}
