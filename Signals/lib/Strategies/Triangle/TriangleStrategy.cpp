#include "TriangleStrategy.h"
#include "Geometry/Validation.h"

#include <sstream>

TriangleStrategy::TriangleStrategy(std::array<shapes::Point, 3> vertices)
	: m_vertices(vertices)
{
}

void TriangleStrategy::Draw(gfx::ICanvas& canvas, gfx::Color color) const
{
	canvas.SetColor(color);
	canvas.DrawPolygon({ m_vertices.begin(), m_vertices.end() });
	canvas.Flush();
}

void TriangleStrategy::Move(double dx, double dy)
{
	for (auto& verticle : m_vertices)
	{
		verticle = { verticle.x + dx, verticle.y + dy };
	}
}

bool TriangleStrategy::ContainsPoint(const shapes::Point& point) const
{
	// Знак косого произведения (b - a) x (p - a) показывает, с какой стороны от ребра ab лежит точка.
	// Точка внутри, если она не лежит по разные стороны от рёбер (ноль — точка на ребре)
	const auto skew = [&point](const shapes::Point& a, const shapes::Point& b) {
		return (b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x);
	};

	const double skew1 = skew(m_vertices[0], m_vertices[1]);
	const double skew2 = skew(m_vertices[1], m_vertices[2]);
	const double skew3 = skew(m_vertices[2], m_vertices[0]);

	const bool hasNegative = skew1 < 0 || skew2 < 0 || skew3 < 0;
	const bool hasPositive = skew1 > 0 || skew2 > 0 || skew3 > 0;
	return !(hasNegative && hasPositive);
}

shapes::Rect TriangleStrategy::GetBounds() const
{
	auto [minX, maxX] = std::minmax({ m_vertices[0].x, m_vertices[1].x, m_vertices[2].x });
	auto [minY, maxY] = std::minmax({ m_vertices[0].y, m_vertices[1].y, m_vertices[2].y });

	return { minX, minY, maxX - minX, maxY - minY };
}

void TriangleStrategy::SetBounds(const shapes::Rect& bounds)
{
	shapes::detail::ValidateBounds(bounds);

	const auto oldBounds = GetBounds();

	for (auto& vertex : m_vertices)
	{
		vertex.x = oldBounds.width == 0 ? bounds.left
										: bounds.left + (vertex.x - oldBounds.left) / oldBounds.width * bounds.width;
		vertex.y = oldBounds.height == 0 ? bounds.top
										 : bounds.top + (vertex.y - oldBounds.top) / oldBounds.height * bounds.height;
	}
}

std::string TriangleStrategy::GetType() const { return "triangle"; }

std::string TriangleStrategy::GetParameters() const
{
	std::ostringstream output;
	output << m_vertices[0].x << ' ' << m_vertices[0].y << ' ' << m_vertices[1].x << ' ' << m_vertices[1].y << ' '
		   << m_vertices[2].x << ' ' << m_vertices[2].y;
	return output.str();
}
