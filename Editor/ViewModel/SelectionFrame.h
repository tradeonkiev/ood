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

std::array<shapes::Rect, 4> GetMarkerRects(const shapes::Rect& frame);
std::optional<Corner> HitMarker(const shapes::Rect& frame, const shapes::Point& point);
shapes::Rect ResizeFrame(const shapes::Rect& frame, Corner corner, const shapes::Point& point);

} // namespace editor
