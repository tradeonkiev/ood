#include "TriangleShape.h"

namespace editor
{

std::string TriangleShape::GetType() const { return "triangle"; }

bool TriangleShape::ContainsPoint(const Point& point) const
{
	const auto [a, b, c] = GetVertices();
	const auto side = [&point](const Point& from, const Point& to) {
		return (to.x - from.x) * (point.y - from.y) - (to.y - from.y) * (point.x - from.x);
	};

	const double side1 = side(a, b);
	const double side2 = side(b, c);
	const double side3 = side(c, a);

	const bool hasNegative = side1 < 0 || side2 < 0 || side3 < 0;
	const bool hasPositive = side1 > 0 || side2 > 0 || side3 > 0;

	return !(hasNegative && hasPositive);
}

void TriangleShape::Draw(ICanvas& canvas) const
{
	const auto vertices = GetVertices();
	canvas.SetFillColor(GetStyle().fillColor);
	canvas.SetStrokeColor(GetStyle().strokeColor);
	canvas.DrawPolygon({ vertices.begin(), vertices.end() });
}

std::array<Point, 3> TriangleShape::GetVertices() const
{
	const Rect bounds = GetBounds();

	return { Point{ bounds.left + bounds.width / 2, bounds.top }, Point{ bounds.Right(), bounds.Bottom() },
		Point{ bounds.left, bounds.Bottom() } };
}

} // namespace editor
