#include "EllipseShape.h"

namespace editor
{

std::string EllipseShape::GetType() const { return "ellipse"; }

bool EllipseShape::ContainsPoint(const Point& point) const
{
	const Rect bounds = GetBounds();
	const double radiusX = bounds.width / 2;
	const double radiusY = bounds.height / 2;
	const double dx = (point.x - (bounds.left + radiusX)) / radiusX;
	const double dy = (point.y - (bounds.top + radiusY)) / radiusY;
	return dx * dx + dy * dy <= 1;
}

void EllipseShape::Draw(ICanvas& canvas) const
{
	const Rect bounds = GetBounds();
	canvas.SetColor(GetColor());
	canvas.DrawEllipse(
		{ bounds.left + bounds.width / 2, bounds.top + bounds.height / 2 }, bounds.width / 2, bounds.height / 2);
}

} // namespace editor
