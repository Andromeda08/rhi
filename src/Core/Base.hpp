#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <set>
#include <vector>

#include <fmt/format.h>
#include <fmt/color.h>

#include "EnumFlags.hpp"

#define DISABLE_COPY_CTOR(TYPE)         \
TYPE(const TYPE&) = delete;             \
TYPE& operator=(const TYPE&) = delete;


#define DEF_PRIMARY_CTOR(TYPE, ...)                         \
TYPE(__VA_ARGS__);                                          \
static std::shared_ptr<TYPE> create##TYPE(__VA_ARGS__);