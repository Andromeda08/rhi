#pragma once

#include <type_traits>

// Toggle the use of namespace for the RHI by (un)defining this macro.
// #define rhi_USE_NAMESPACE

// Set the name of the namespace used for the RHI here.
#ifndef rhi_NAMESPACE
    #define rhi_NAMESPACE rhi
#endif

#ifdef rhi_USE_NAMESPACE
    #define rhi_BEGIN_NAMESPACE namespace rhi_NAMESPACE {
    #define rhi_END_NAMESPACE }
#else
    #define rhi_BEGIN_NAMESPACE
    #define rhi_END_NAMESPACE
#endif

#define XSTR(A) #A
#define STR(A) XSTR(A)

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

#define DEF_AS_CONVERT(TYPE)                                \
template <typename T>                                       \
T* as() {                                                   \
    static_assert(                                          \
        std::is_base_of_v<TYPE, T>,                         \
        "Template parameter T must be a type of " #TYPE);   \
    return dynamic_cast<T*>(this);                          \
}
