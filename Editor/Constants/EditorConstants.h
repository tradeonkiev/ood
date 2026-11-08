#pragma once

#include "Canvas/Color.h"

namespace editor
{

inline constexpr double MaxTime = 10;
inline constexpr double MinDuration = 0.1;

inline constexpr Color DefaultFillColor{ 111, 222, 33 };
inline constexpr Color DefaultStrokeColor{ 0, 0, 0 };

inline constexpr double MarkerSize = 10;

inline constexpr double MinZoom = 0.1;
inline constexpr double MaxZoom = 10;

} // namespace editor
