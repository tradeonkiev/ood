#pragma once

namespace editor
{

struct Point
{
	double x = 0;
	double y = 0;

	bool operator==(const Point&) const = default;
};

struct Rect
{
	double left = 0;
	double top = 0;
	double width = 0;
	double height = 0;

	double Right() const { return left + width; }
	double Bottom() const { return top + height; }

	bool operator==(const Rect&) const = default;
};

} // namespace editor
