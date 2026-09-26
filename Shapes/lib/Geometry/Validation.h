#pragma once

#include <cmath>
#include <stdexcept>
#include "Geometry.h"

namespace shapes::detail {
    // inline void ValidatePoint(const Point &point) {
    //     if (point.x < 0 || point.y < 0) {
    //         throw std::invalid_argument("Bounds size must not be negative");
    //     }
    // }

    inline void ValidateBounds(const Rect &bounds) {
        // ValidatePoint({bounds.left, bounds.top});
        if (bounds.width < 0 || bounds.height < 0) {
            throw std::invalid_argument("Bounds size must not be negative");
        }
    }

}; // namespace shapes::detail
