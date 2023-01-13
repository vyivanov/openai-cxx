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

struct RestConnector {
    enum struct Request { POST };

    struct Response {
        std::uint16_t code;
        std::string body;
    };

    virtual RestConnector& appendRoutePath(std::string&& value) = 0;
    virtual RestConnector& addHeaderField(std::string&& name, std::string&& value) = 0;
    virtual RestConnector& setBodyContent(std::string&& data) = 0;

    virtual std::future<Response> submitRequest(Request type) = 0;

    virtual ~RestConnector() = default;
};

std::future<RestConnector::Response> convert(std::future<cpr::Response> future)
{
    assert(future.valid());
    return cpr::async(
        [](auto& future) -> RestConnector::Response {
            auto http_response = future.get();
            return {
                .code = static_cast<decltype(RestConnector::Response::code)>(http_response.status_code),
                .body = std::move(http_response.text),
            };
        },
        std::move(future)
    );
}

auto apiKey() -> std::string
{
    const char* const ENV_VAR = "OPENAI_API_KEY";
    const char* const API_KEY = std::getenv(ENV_VAR);

    if (not API_KEY)
    {
        throw std::invalid_argument{"Provide your '"s + ENV_VAR + "' variable to use it's services."};
    }

    return API_KEY;
}

class HttpConnector final: public RestConnector {
public:
    explicit HttpConnector(std::string&& url, std::optional<std::string>&& bearer={})
        : m_url(std::move(url))
    {
        assert(m_url.str().length() > 0);
        assert(m_url.str().back() != '/');

        assert(m_header.empty());
        assert(m_body.str().empty());

        if (bearer.has_value()) {
            addHeaderField("Authorization", "Bearer "s + (*bearer));
        }
    }

    RestConnector& appendRoutePath(std::string&& value) override
    {
        assert(not value.empty());
        assert(value.front() == '/');
        assert(value.back() != '/');

        m_url = m_url.str() + std::move(value);
        return (*this);
    }

    RestConnector& addHeaderField(std::string&& name, std::string&& value) override
    {
        assert(not name.empty());
        assert(not value.empty());

        [[maybe_unused]]
        const auto [_, ok] = m_header.emplace(
            std::pair{std::move(name), std::move(value)}
        );

        assert(ok);
        return (*this);
    }

    RestConnector& setBodyContent(std::string&& data) override
    {
        assert(not data.empty());

        m_body = std::move(data);
        return (*this);
    }

    std::future<Response> submitRequest(const Request type) override
    {
        auto session = std::make_shared<cpr::Session>();

        session->SetOption(m_url);
        session->SetOption(m_header);

        session->SetOption(std::move(m_body));
        m_body = cpr::Body{};

        if (Request::POST == type) {
            return convert(session->PostAsync());
        }

        std::abort();
    }

private:
    cpr::Url m_url;
    cpr::Header m_header;
    cpr::Body m_body;
};

}

namespace Openai::Impl {

class Completion {
public:
    explicit Completion(std::unique_ptr<RestConnector> connector): m_connector(std::move(connector))
    {
        assert(m_connector);

        (*m_connector)
            .appendRoutePath("/completions")
            .addHeaderField("Content-Type", "application/json");
    }

    void model(std::string_view idx)
    {
        m_model = idx;
    }

    void prompt(std::string_view txt)
    {
        m_prompt = txt;
    }

    std::future<RestConnector::Response> submit()
    {
        return m_connector.operator*()
                .setBodyContent(to_json().dump())
                .submitRequest(RestConnector::Request::POST);
    }

    class Result;

private:
    std::unique_ptr<RestConnector> m_connector;

    TO_JSON_METHOD(model, prompt);

    std::optional<std::string> m_model;
    std::optional<std::string> m_prompt;
};

class Completion::Result {
public:
    struct Choice {
        std::string text;
    };
    explicit Result(std::future<RestConnector::Response> response): m_response(std::move(response)) {

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
    std::future<RestConnector::Response> m_response;
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

    auto completion = Openai::Impl::Completion{
        std::make_unique<Openai::Impl::HttpConnector>("https://api.openai.com/v1", Openai::Impl::apiKey())};

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
