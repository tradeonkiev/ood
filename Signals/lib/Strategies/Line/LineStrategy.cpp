#include "LineStrategy.h"
#include "Geometry/Validation.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

LineStrategy::LineStrategy(shapes::Point start, shapes::Point end)
	: m_start(start)
	, m_end(end)
{
}

void LineStrategy::Draw(gfx::ICanvas& canvas, gfx::Color color) const
{
	canvas.SetColor(color);
	canvas.MoveTo(m_start);
	canvas.LineTo(m_end);
	canvas.Flush();
}

void LineStrategy::Move(double dx, double dy)
{
	m_start = { m_start.x + dx, m_start.y + dy };
	m_end = { m_end.x + dx, m_end.y + dy };
}

bool LineStrategy::ContainsPoint(const shapes::Point& point) const
{
	constexpr double HitTolerance = 3;

	// Расстояние от точки до отрезка: проецируем точку на прямую и зажимаем проекцию в пределах отрезка
	const double dx = m_end.x - m_start.x;
	const double dy = m_end.y - m_start.y;
	const double lengthSquared = dx * dx + dy * dy;
	const double t = lengthSquared == 0
		? 0
		: std::clamp(((point.x - m_start.x) * dx + (point.y - m_start.y) * dy) / lengthSquared, 0.0, 1.0);

	const double nearestX = m_start.x + t * dx;
	const double nearestY = m_start.y + t * dy;
	return std::hypot(point.x - nearestX, point.y - nearestY) <= HitTolerance;
}

shapes::Rect LineStrategy::GetBounds() const
{
	return { std::min(m_start.x, m_end.x), std::min(m_start.y, m_end.y), std::abs(m_end.x - m_start.x),
		std::abs(m_end.y - m_start.y) };
}

void LineStrategy::SetBounds(const shapes::Rect& bounds)
{
	shapes::detail::ValidateBounds(bounds);
	const auto oldBounds = GetBounds();

	const auto transform = [&](shapes::Point point) -> shapes::Point {
		return {
			oldBounds.width == 0 ? bounds.left
								 : bounds.left + (point.x - oldBounds.left) / oldBounds.width * bounds.width,
			oldBounds.height == 0 ? bounds.top
								  : bounds.top + (point.y - oldBounds.top) / oldBounds.height * bounds.height,
		};
	};
	m_start = transform(m_start);
	m_end = transform(m_end);
}

std::string LineStrategy::GetType() const { return "line"; }

std::string LineStrategy::GetParameters() const
{
	std::ostringstream output;
	output << m_start.x << ' ' << m_start.y << ' ' << m_end.x << ' ' << m_end.y;
	return output.str();
}
