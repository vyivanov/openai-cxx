#pragma once

#include <string>
#include <vector>

namespace Openai {

class Completion {
public:
    Completion();

    Completion& model(std::string id) noexcept;

    Completion& prompt(std::string text) noexcept;
    Completion& prompt(std::vector<std::string> text_list) noexcept;

    Completion& temperature(float sampling) noexcept;

    Completion& echo() noexcept;

    class Result;
    Result create();
};

class Completion::Result {

};

}
