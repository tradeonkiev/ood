#include "SelectionFrame.h"

#include "Constants/EditorConstants.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace editor
{

std::array<Point, 4> GetCorners(const Rect& frame)
{
	return { Point{ frame.left, frame.top }, Point{ frame.Right(), frame.top },
		Point{ frame.Right(), frame.Bottom() }, Point{ frame.left, frame.Bottom() } };
}

Point GetOppositeCorner(const Rect& frame, Corner corner)
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

Point GetCorner(const Rect& frame, Corner corner) { return GetCorners(frame)[static_cast<int>(corner)]; }

std::array<Rect, 4> GetMarkerRects(const Rect& frame)
{
	std::array<Rect, 4> markers;
	const auto corners = GetCorners(frame);
	for (int i = 0; i < corners.size(); i++)
	{
		markers[i] = { corners[i].x - MarkerSize / 2, corners[i].y - MarkerSize / 2, MarkerSize, MarkerSize };
	}
	return markers;
}

std::optional<Corner> HitMarker(const Rect& frame, const Point& point)
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

Rect MakeFrame(const Point& fixed, const Point& dragged)
{
	const auto [left, width] = GetSpan(fixed.x, dragged.x);
	const auto [top, height] = GetSpan(fixed.y, dragged.y);
	return { left, top, width, height };
}

Rect ResizeFrame(const Rect& frame, Corner corner, const Point& point)
{
	return MakeFrame(GetOppositeCorner(frame, corner), point);
}

} // namespace editor
