#include <cassert>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <utility>
#include <variant>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

struct Result {
    double a;
};

int main()
{
    const char* const api_key = std::getenv("OPENAI_API_KEY");
    assert(api_key);

    auto request_queue = std::queue<
        std::pair<
            cpr::AsyncResponse,
            std::variant<
                std::function<void(Result)>, std::promise<Result>
            >
        >
    >{};

    auto async = cpr::PostAsync(
        cpr::Url{"https://api.openai.com/v1/completions"},
        cpr::Bearer{std::string{api_key}},
        cpr::Header{
            {"Content-Type", "application/json"}
        },
        cpr::Body{
            nlohmann::json{
                {"model", "text-davinci-003"},
                {"prompt", "I wanna to"},
            }.dump()
        }
    );

    auto res = std::variant<
        std::function<void(Result)>, std::promise<Result>>{};

    res = std::promise<Result>{};
    auto f = std::get<1>(res).get_future();
    request_queue.emplace(std::pair{std::move(async), std::move(res)});

    std::cout << R"(Waiting ... )";
    std::cout.flush();

    // Back side

    auto [a, r] = std::move(request_queue.front());
    request_queue.pop();

    const auto response = a.get();
    std::get<1>(r).set_value(Result{.a=response.elapsed});
    std::cout << f.get().a << " sec" << '\n';

    assert(not response.error);
    assert(not response.text.empty());

    assert(response.status_code == cpr::status::HTTP_OK);
    assert(response.header.contains("content-type"));

    std::cout << '\n' << response.text << '\n';
}
