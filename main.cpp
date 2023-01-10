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

namespace Openai {

class RestInterface {
public:
    RestInterface()
        : m_url{"https://api.openai.com/v1"}
        , m_token{"OPENAI_API_KEY"}  // XXX: Token is not default constructible.
    {
        const char* const api_key = std::getenv(m_token.GetToken());
        if (not api_key) {
            throw std::invalid_argument{m_token.GetToken() + std::string{" is not provided"}};
        }
        m_token = cpr::Bearer{api_key};
    }
    void append_url(std::string&& append) {
        m_url = m_url.str() + std::move(append);
    }
    void update_header(std::string&& key, std::string&& val) {
        m_header.emplace(std::pair{std::move(key), std::move(val)});
    }
    void set_body(std::string&& set) {
        m_body = std::move(set);
    }
    void make_post() {
        m_request = cpr::PostAsync(m_url, m_token, m_header, m_body);
    }
    cpr::Response get() {
        return m_request.get();
    }
private:
    cpr::Url m_url;
    cpr::Bearer m_token;
    cpr::Header m_header;
    cpr::Body m_body;

    cpr::AsyncResponse m_request;
};

class Completion {
public:
    Completion() {
        rest.append_url("/completions");
        rest.update_header("Content-Type", "application/json");
    }
    Completion& with_model(std::string_view idx) {
        model = idx;
        return (*this);
    }
    Completion& with_prompt(std::string_view txt) {
        prompt = txt;
        return (*this);
    }
    Completion&& create() {
        rest.set_body(to_json().dump());
        rest.make_post();
        return std::move(*this);
    }
    cpr::Response get() {
        return rest.get();
    }
private:
    TO_JSON_METHOD(model, prompt);

    std::optional<std::string> model;
    std::optional<std::string> prompt;

    RestInterface rest;
};

}

int main()
{
    auto model = Openai::Completion{}
        .with_model("text-davinci-003")
        .with_prompt("I wanna to")
        .create();

    std::cout << R"(Waiting ... )";
    std::cout.flush();

    auto response = model.get();

    std::cout << response.elapsed << " sec " << '\n';
    std::cout << response.status_code << '\n';

    assert(not response.error);
    assert(not response.text.empty());

    std::cout << '\n' << response.text << '\n';
}
