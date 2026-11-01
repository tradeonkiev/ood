#pragma once

#include "Geometry/Geometry.h"

#include <array>
#include <optional>

namespace editor
{

enum class Corner
{
	TopLeft,
	TopRight,
	BottomRight,
	BottomLeft,
};

Point GetCorner(const Rect& frame, Corner corner);
std::array<Rect, 4> GetMarkerRects(const Rect& frame);
std::optional<Corner> HitMarker(const Rect& frame, const Point& point);
Rect MakeFrame(const Point& fixed, const Point& dragged);
Rect ResizeFrame(const Rect& frame, Corner corner, const Point& point);

} // namespace editor
