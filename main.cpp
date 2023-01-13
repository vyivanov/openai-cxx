#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using namespace std::string_literals;

template<typename T>
struct is_optional: std::false_type {};

template<typename T>
struct is_optional<std::optional<T>>: std::true_type {};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

#define OPTIONAL_TO_JSON(o)                 \
static_assert(is_optional_v<decltype(m_##o)>);  \
if (m_##o.has_value()) { j[#o] = (*m_##o); }

#define TO_JSON_METHOD(...)                             \
nlohmann::json to_json() const {                        \
    auto j = nlohmann::json{};                          \
    NLOHMANN_JSON_PASTE(OPTIONAL_TO_JSON, __VA_ARGS__)  \
    return j;                                           \
}

namespace Openai::Impl {

struct RestInterface {
    enum struct Request { POST };

    struct Response {
        std::uint16_t code;
        std::string body;
    };

    virtual RestInterface& set_route_path(std::string&& value) = 0;
    virtual RestInterface& add_header_field(std::string&& name, std::string&& value) = 0;
    virtual RestInterface& set_body_content(std::string&& data) = 0;

    virtual std::future<Response> submit_request(Request type) = 0;

    virtual ~RestInterface() = default;
};

std::future<RestInterface::Response> convert(std::future<cpr::Response> future)
{
    assert(future.valid());
    return cpr::async(
        [](auto& future) -> RestInterface::Response {
            auto http_response = future.get();
            return {
                .code = static_cast<decltype(RestInterface::Response::code)>(http_response.status_code),
                .body = std::move(http_response.text),
            };
        },
        std::move(future)
    );
}

auto get_api_key() -> std::string
{
    const char* const ENV_VAR = "OPENAI_API_KEY";
    const char* const API_KEY = std::getenv(ENV_VAR);

    if (not API_KEY) {
        throw std::invalid_argument{"Provide your '"s + ENV_VAR + "' variable to use it's services."};
    }

    return API_KEY;
}

class RestApi: public RestInterface {
public:
    explicit RestApi(std::string&& base = "https://api.openai.com/v1")
        : m_token(get_api_key())
        , m_url(std::move(base))
    {
        assert(strlen(m_token.GetToken()) > 0);

        assert(m_url.str().length() > 0);
        assert(m_url.str().back() != '/');

        assert(m_header.empty());
        assert(m_body.str().empty());
    }

    RestInterface& set_route_path(std::string&& value) override
    {
        m_url = m_url.str() + std::move(value);

        return (*this);
    }

    RestInterface& add_header_field(std::string&& name, std::string&& value) override
    {
        [[maybe_unused]]
        const auto [_, ok] = m_header.emplace(
            std::pair{std::move(name), std::move(value)}
        );
        assert(ok);

        return (*this);
    }

    RestInterface& set_body_content(std::string&& data) override
    {
        m_body = std::move(data);

        return (*this);
    }

    std::future<Response> submit_request(const Request type) override
    {
        if (type == Request::POST)
        {
            return convert(cpr::PostAsync(m_url, m_token, m_header, m_body));
        }

        std::abort();
    }

private:
    const cpr::Bearer m_token;

    cpr::Url m_url;
    cpr::Header m_header;
    cpr::Body m_body;
};

}

namespace Openai::Impl {

class Completion {
public:
    explicit Completion(std::unique_ptr<RestInterface> rest_api): m_rest_api(std::move(rest_api))
    {
        m_rest_api.operator*()
                .set_route_path("/completions")
                  .add_header_field("Content-Type", "application/json");
    }

    void model(std::string_view idx)
    {
        m_model = idx;
    }

    void prompt(std::string_view txt)
    {
        m_prompt = txt;
    }

    std::future<RestInterface::Response> submit()
    {
        return m_rest_api.operator*()
                         .set_body_content(to_json().dump())
                         .submit_request(RestInterface::Request::POST);
    }

    class Result;

private:
    std::unique_ptr<RestInterface> m_rest_api;

    TO_JSON_METHOD(model, prompt);

    std::optional<std::string> m_model;
    std::optional<std::string> m_prompt;
};

class Completion::Result {
public:
    struct Choice {
        std::string text;
    };
    explicit Result(std::future<RestInterface::Response> response): m_response(std::move(response)) {

    }
    std::optional<std::time_t> created()
    {
        parse_response();
        return m_created;
    }
    std::optional<Choice> choices() {
        parse_response();
        return m_choices;
    }
private:
    void parse_response() {
        if (m_response.valid()) {
            auto out = m_response.get();
            m_created = nlohmann::json::parse(out.body).value("created", 0);
            m_choices = Choice{.text=nlohmann::json::parse(out.body).at("choices").at(0).value("text", "")};
        }
    }
    std::future<RestInterface::Response> m_response;
    std::optional<std::time_t> m_created;
    std::optional<Choice> m_choices;
};

}

int main()
{
    auto test = nlohmann::json::parse(
R"json(
{
    "id": "cmpl-6XtQ30LBc9RimfVYTRd2wpselIszg",
    "object": "text_completion",
    "created": 1673535875,
    "model": "text-davinci-003",
    "choices":
    [
        {
            "text": " go shopping\n\nSounds like fun! Where would you like to go shopping?",
            "index": 0,
            "logprobs": null,
            "finish_reason": "length"
        }
    ],
    "usage":
    {
        "prompt_tokens": 3,
        "completion_tokens": 16,
        "total_tokens": 19
    }
}
)json"
    );

//    int out{};
//    test.at("created").get_to(out);
//    std::cout << test << '\n';

    auto completion = Openai::Impl::Completion{std::make_unique<Openai::Impl::RestApi>()};
    completion.model("text-davinci-003");
    completion.prompt("I wanna to");
//
    auto result = completion.submit();
//
    std::cout << R"(Waiting ... )";
    std::cout.flush();

//    auto response = result.get();
//    std::cout << response.code << '\n';

    std::cout << Openai::Impl::Completion::Result{std::move(result)}.choices().value().text << '\n';

//    std::cout << response.elapsed << " sec " << '\n';
//    std::cout << response.status_code << '\n';
//
//    assert(not response.error);
//    assert(not response.text.empty());
//
//    std::cout << '\n' << response.body << '\n';
}
