#pragma once

#define DISABLE_COPY_CTOR(TYPE)         \
TYPE(const TYPE&) = delete;             \
TYPE& operator=(const TYPE&) = delete;  \
TYPE(const TYPE&&) = delete;            \
TYPE& operator=(const TYPE&&) = delete;

#define DEF_PRIMARY_CTOR(TYPE, ...)                         \
TYPE(__VA_ARGS__);                                          \
static std::unique_ptr<TYPE> create##TYPE(__VA_ARGS__);

#define DEF_PRIMARY_SH_CTOR(TYPE, ...)                      \
TYPE(__VA_ARGS__);                                          \
static std::shared_ptr<TYPE> create##TYPE(__VA_ARGS__);

#define XSTR(A) #A
#define STR(A) XSTR(A)

#define DEF_AS_CONVERT(TYPE)                                \
template <typename T>                                       \
T* as() {                                                   \
    static_assert(                                          \
        std::is_base_of_v<TYPE, T>,                         \
        "Template parameter T must be a type of " #TYPE);   \
    return dynamic_cast<T*>(this);                          \
}

#define ENUM_FLAGS(Enum) \
    inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
    inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
    inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
    inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
    inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
    inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }
