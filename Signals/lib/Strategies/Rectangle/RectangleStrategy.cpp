#include "RectangleStrategy.h"
#include "Geometry\Validation.h"

#include <sstream>

RectangleStrategy::RectangleStrategy(shapes::Rect bounds) { SetBounds(bounds); }

RectangleStrategy::RectangleStrategy(shapes::Point topLeft, double width, double height)
	: RectangleStrategy(shapes::Rect{ topLeft.x, topLeft.y, width, height })
{
}

void RectangleStrategy::Draw(gfx::ICanvas& canvas, gfx::Color color) const
{
	canvas.SetColor(color);
	canvas.DrawRectangle(m_bounds);
	canvas.Flush();
}

void RectangleStrategy::Move(double dx, double dy)
{
	m_bounds = { m_bounds.left + dx, m_bounds.top + dy, m_bounds.width, m_bounds.height };
}

bool RectangleStrategy::ContainsPoint(const shapes::Point& point) const
{
	return point.x >= m_bounds.left && point.x <= m_bounds.left + m_bounds.width && point.y >= m_bounds.top && point.y <= m_bounds.top + m_bounds.height;
}

shapes::Rect RectangleStrategy::GetBounds() const { return m_bounds; }

void RectangleStrategy::SetBounds(const shapes::Rect& bounds)
{
	shapes::detail::ValidateBounds(bounds);
	m_bounds = bounds;
}

std::string RectangleStrategy::GetType() const { return "rectangle"; }

std::string RectangleStrategy::GetParameters() const
{
	std::ostringstream output;
	output << m_bounds.left << ' ' << m_bounds.top << ' ' << m_bounds.width << ' ' << m_bounds.height;
	return output.str();
}
