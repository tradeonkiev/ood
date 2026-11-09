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

inline Rect MapRect(const Rect& rect, const Rect& from, const Rect& to)
{
	const double scaleX = from.width > 0 ? to.width / from.width : 1;
	const double scaleY = from.height > 0 ? to.height / from.height : 1;
	return { to.left + (rect.left - from.left) * scaleX, to.top + (rect.top - from.top) * scaleY, rect.width * scaleX,
		rect.height * scaleY };
}

} // namespace editor
