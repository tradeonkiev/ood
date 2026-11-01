#include "RectangleShape.h"

namespace editor
{

std::string RectangleShape::GetType() const { return "rectangle"; }

bool RectangleShape::ContainsPoint(const Point& point) const
{
	const Rect bounds = GetBounds();

	return point.x >= bounds.left && point.x <= bounds.Right() && point.y >= bounds.top && point.y <= bounds.Bottom();
}

void RectangleShape::DrawGeometry(ICanvas& canvas) const
{
	canvas.DrawRectangle(GetBounds());
}

} // namespace editor
