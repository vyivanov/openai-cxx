#include <cassert>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

template<typename T>
struct is_optional: std::false_type {};

template<typename T>
struct is_optional<std::optional<T>>: std::true_type {};

template<typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

#define OPTIONAL_TO_JSON(o)                 \
static_assert(is_optional_v<decltype(o)>);  \
if (o.has_value()) { j[#o] = (*o); }

#define TO_JSON_METHOD(...)                             \
nlohmann::json to_json() const {                        \
    auto j = nlohmann::json{};                          \
    NLOHMANN_JSON_PASTE(OPTIONAL_TO_JSON, __VA_ARGS__)  \
    return j;                                           \
}

namespace Openai::Impl {

class RestInterface {
public:
    enum class Request {
        POST,
    };

    struct Response {
        uint16_t code;
        std::string body;
    };

    virtual RestInterface& set_endpoint_path(const std::string& path) = 0;
    virtual RestInterface& add_header_field(const std::string& name, const std::string& value) = 0;
    virtual RestInterface& set_body_content(const std::string& data) = 0;

    virtual std::future<Response> submit_request(Request type) = 0;

    virtual ~RestInterface() = default;
};

class RestApi: public RestInterface {
public:
    RestApi(): m_token{"OPENAI_API_KEY"}  // XXX: Token is not default constructible.
    {
        const char* const api_key = std::getenv(m_token.GetToken());
        if (not api_key) {
            throw std::invalid_argument{m_token.GetToken() + std::string{" is not provided"}};
        }
        m_token = cpr::Bearer{api_key};
        m_url = std::string{BASE_URL};
    }

    RestInterface& set_endpoint_path(const std::string& path) override
    {
        m_url += path;
        return (*this);
    }

    RestInterface& add_header_field(const std::string& name, const std::string& value) override
    {
        m_header.emplace(std::pair{name, value});
        return (*this);
    }

    RestInterface& set_body_content(const std::string& data) override
    {
        m_body = data;
        return (*this);
    }

    std::future<Response> submit_request(Request /*type*/) override
    {
        auto f_response = cpr::PostAsync(m_url, m_token, m_header, m_body);
        auto p_response = std::promise<Response>{};

        auto f_client = p_response.get_future();

        cpr::async(
            [](std::future<cpr::Response>& f_response, std::promise<Response>& p_response) {
                auto cpr_response = f_response.get();
                auto api_response = Response {
                    .code = static_cast<decltype(Response::code)>(cpr_response.status_code),
                    .body = std::move(cpr_response.text),
                };
                p_response.set_value(std::move(api_response));
            },
            std::move(f_response), std::move(p_response)
        );

        return f_client;
    }

private:
    constexpr static std::string_view BASE_URL{"https://api.openai.com/v1"};

    cpr::Url m_url;
    cpr::Bearer m_token;
    cpr::Header m_header;
    cpr::Body m_body;
};

}

namespace Openai::Impl {

class Completion {
public:
    explicit Completion(std::unique_ptr<RestInterface>&& rest_api):
        m_rest_api(std::move(rest_api))
    {
        m_rest_api.operator*()
                  .set_endpoint_path("/completions")
                  .add_header_field("Content-Type", "application/json");
    }

    Completion& with_model(std::string_view idx)
    {
        model = idx; return (*this);
    }

    Completion& with_prompt(std::string_view txt)
    {
        prompt = txt; return (*this);
    }

    std::future<RestInterface::Response> async()
    {
        return m_rest_api.operator*()
                         .set_body_content(to_json().dump())
                         .submit_request(RestInterface::Request::POST);
    }

private:
    std::unique_ptr<RestInterface> m_rest_api;

    TO_JSON_METHOD(model, prompt);

    std::optional<std::string> model;
    std::optional<std::string> prompt;
};

}

int main()
{
    auto model = Openai::Impl::Completion{std::make_unique<Openai::Impl::RestApi>()}
            .with_model("text-davinci-003")
            .with_prompt("I wanna to")
            .async();

    std::cout << R"(Waiting ... )";
    std::cout.flush();

    auto response = model.get();
    std::cout << response.code << '\n';

//    std::cout << response.elapsed << " sec " << '\n';
//    std::cout << response.status_code << '\n';
//
//    assert(not response.error);
//    assert(not response.text.empty());
//
    std::cout << '\n' << response.body << '\n';
}
