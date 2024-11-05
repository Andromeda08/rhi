#pragma once

#define DISABLE_COPY_CTOR(TYPE)         \
TYPE(const TYPE&) = delete;             \
TYPE& operator=(const TYPE&) = delete;

#define DEF_PRIMARY_CTOR(TYPE, ...)                         \
TYPE(__VA_ARGS__);                                          \
static std::unique_ptr<TYPE> create##TYPE(__VA_ARGS__);

#define DEF_PRIMARY_SH_CTOR(TYPE, ...)                      \
TYPE(__VA_ARGS__);                                          \
static std::shared_ptr<TYPE> create##TYPE(__VA_ARGS__);