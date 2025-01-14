#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include <fmt/format.h>
#include <fmt/color.h>

#include "EnumFlags.hpp"

#define PRINT_PLACEHOLDER(MSG) \
fmt::println("{}", styled(fmt::format("[Temp] {}", MSG), fg(fmt::color::light_yellow)))

#define RHI_PRINTLN(MESSAGE) \
fmt::println("[{}] {}", styled("Elysia", fg(fmt::color::middle_purple)), MESSAGE)