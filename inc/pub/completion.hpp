#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Openai {
namespace Impl::Completion { class Completion; class Result; }

class Completion {
public:
    Completion();
    virtual ~Completion() noexcept;

    Completion(const Completion&) = delete;
    Completion& operator=(const Completion&) = delete;

    Completion(Completion&&) noexcept;
    Completion& operator=(Completion&&) noexcept;

    Completion& model(std::string id) noexcept;

    Completion& prompt(std::string text) noexcept;
    Completion& prompt(std::vector<std::string> text_list) noexcept;

    Completion& temperature(float sampling) noexcept;

    Completion& echo() noexcept;

    class Result;
    Result create();

private:
    std::unique_ptr<Impl::Completion::Completion> m_pimpl;
};

class Completion::Result {
friend class Completion;
public:
    Result();
    virtual ~Result() noexcept;

    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;

    Result(Result&&) noexcept;
    Result& operator=(Result&&) noexcept;

private:
    std::unique_ptr<Impl::Completion::Result> m_pimpl;
};

}
