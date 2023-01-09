#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

int main()
{
    const char* const api_key = std::getenv("OPENAI_API_KEY");
    assert(api_key);

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

    std::cout << R"(Waiting ... )";
    std::cout.flush();

    const auto response = async.get();
    std::cout << response.elapsed << " sec" << '\n';

    assert(not response.error);
    assert(not response.text.empty());

    assert(response.status_code == cpr::status::HTTP_OK);
    assert(response.header.contains("content-type"));

    std::cout << '\n' << response.text << '\n';
}
