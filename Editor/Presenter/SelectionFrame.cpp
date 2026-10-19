#include "SelectionFrame.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace editor
{

std::array<shapes::Point, 4> GetCorners(const shapes::Rect& frame)
{
	return { shapes::Point{ frame.left, frame.top }, shapes::Point{ frame.Right(), frame.top },
		shapes::Point{ frame.Right(), frame.Bottom() }, shapes::Point{ frame.left, frame.Bottom() } };
}

shapes::Point GetOppositeCorner(const shapes::Rect& frame, Corner corner)
{
	switch (corner)
	{
	case Corner::TopLeft:
		return { frame.Right(), frame.Bottom() };
	case Corner::TopRight:
		return { frame.left, frame.Bottom() };
	case Corner::BottomRight:
		return { frame.left, frame.top };
	case Corner::BottomLeft:
		return { frame.Right(), frame.top };
	}
	return {};
}

std::pair<double, double> GetSpan(double fixed, double dragged)
{
	const double length = std::max(std::abs(dragged - fixed), 20.0);
	const double start = dragged >= fixed ? fixed : fixed - length;
	return { start, length };
}

std::array<shapes::Rect, 4> GetMarkerRects(const shapes::Rect& frame)
{
	std::array<shapes::Rect, 4> markers;
	const auto corners = GetCorners(frame);
	for (int i = 0; i < corners.size(); i++)
	{
		markers[i] = { corners[i].x - 5, corners[i].y - 5, 10, 10 };
	}
	return markers;
}

std::optional<Corner> HitMarker(const shapes::Rect& frame, const shapes::Point& point)
{
	const auto markers = GetMarkerRects(frame);
	for (int i = 0; i < markers.size(); i++)
	{
		const auto& marker = markers[i];
		if (point.x >= marker.left && point.x <= marker.Right() && point.y >= marker.top && point.y <= marker.Bottom())
		{
			return static_cast<Corner>(i);
		}
	}
	return std::nullopt;
}

shapes::Rect ResizeFrame(const shapes::Rect& frame, Corner corner, const shapes::Point& point)
{
	const shapes::Point stabelCorner = GetOppositeCorner(frame, corner);

	const auto [left, width] = GetSpan(stabelCorner.x, point.x);
	const auto [top, height] = GetSpan(stabelCorner.y, point.y);
	return { left, top, width, height };
}

} // namespace editor
