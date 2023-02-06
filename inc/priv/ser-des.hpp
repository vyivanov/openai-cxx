#pragma once

#include <optional>
#include <type_traits>

#include <nlohmann/json.hpp>

template<typename T>
struct is_optional: std::false_type {};

template<typename T>
struct is_optional<std::optional<T>>: std::true_type {};

template<typename... Ts>
struct is_variant: std::false_type {};

template<typename... Ts>
struct is_variant<std::variant<Ts...>>: std::true_type {};

#define DECLARE_TO_JSON(StructT)    \
template <typename T>               \
friend nlohmann::json StructT##_to_json(T&& from_obj)

#define SERIALIZE(field) { \
    auto member = std::move(from_obj.m_##field); \
    if (member.has_value()) {                    \
        using type = std::remove_reference_t<decltype(*member)>;                   \
        if constexpr (is_variant<type>::value) {                   \
            if (std::holds_alternative<std::variant_alternative_t<0, type>>(*member)) {               \
                json[#field] = std::move(std::get<0>(*member)); \
            } else { \
                json[#field] = std::move(std::get<1>(*member)); \
            } \
        } else {                   \
            json[#field] = std::move((*member));      \
        }                       \
    }                       \
}

#define DEFINE_TO_JSON(StructT, ...)                \
template <typename T>                                   \
nlohmann::json StructT##_to_json(T&& from_obj) {   \
    auto json = nlohmann::json{};                       \
    NLOHMANN_JSON_PASTE(SERIALIZE, __VA_ARGS__)         \
    return json;                                        \
}

#define CALL_TO_JSON(StructT, ptr) \
[ptr] { assert(ptr); return StructT##_to_json(*ptr); }()

#define DECLARE_FROM_JSON(ClassT)  \
template <typename T>                   \
friend void ClassT##_from_json(T& to_obj, nlohmann::json&& json)

#define DESERIALIZE(field) { \
    json.at(#field).get_to(to_obj.m_##field);                             \
}

#define DEFINE_FROM_JSON(ClassT, ...)  \
template <typename T>                       \
void ClassT##_from_json(T& to_obj, nlohmann::json&& json) { \
    NLOHMANN_JSON_PASTE(DESERIALIZE, __VA_ARGS__)  \
}

#define CALL_FROM_JSON(ClassT, ptr, json) \
[&,ptr] { assert(ptr); ClassT##_from_json(*ptr, json); }()
