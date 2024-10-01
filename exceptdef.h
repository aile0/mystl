#pragma once

#include <cassert>
#include <stdexcept>

namespace mystl {
#define MYSTL_DEBUG(expr) assert(expr)

#define MYSTL_LENGTH_ERROR_IF(expr, msg) \
    if ((expr)) throw std::length_error(msg)

#define MYSTL_OUT_OF_RANGE_IF(expr, msg) \
    if ((expr)) throw std::out_of_range(msg)

#define MYSTL_RUNTIME_ERROR_IF(expr, msg) \
    if ((expr)) throw std::runtime_error(msg)
}  // namespace mystl