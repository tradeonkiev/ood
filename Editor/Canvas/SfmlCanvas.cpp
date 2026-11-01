#include "SfmlCanvas.h"

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <cmath>
#include <numbers>

namespace editor
{

constexpr float StrokeThickness = 2;

SfmlCanvas::SfmlCanvas(sf::RenderTarget& target, sf::Vector2f origin)
	: m_target(target)
	, m_origin(origin)
{
}

void SfmlCanvas::Clear() { m_target.clear(sf::Color::White); }

sf::Color ToSfmlColor(Color color) { return { color.red, color.green, color.blue }; }

void SfmlCanvas::SetFillColor(Color color) { m_fillColor = ToSfmlColor(color); }

void SfmlCanvas::SetStrokeColor(Color color) { m_strokeColor = ToSfmlColor(color); }

void SfmlCanvas::DrawRectangle(const Rect& bounds)
{
	sf::RectangleShape rectangle({ static_cast<float>(bounds.width), static_cast<float>(bounds.height) });
	rectangle.setPosition(ToWindow({ bounds.left, bounds.top }));
	DrawFilled(rectangle);
}

void SfmlCanvas::DrawPolygon(const std::vector<Point>& vertices)
{
	sf::ConvexShape polygon(vertices.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		polygon.setPoint(i, ToWindow(vertices[i]));
	}
	DrawFilled(polygon);
}

void SfmlCanvas::DrawEllipse(Point center, double radiusX, double radiusY)
{
	constexpr int PointCount = 64;
	sf::ConvexShape ellipse(PointCount);
	for (int i = 0; i < PointCount; i++)
	{
		const double angle = 2 * std::numbers::pi * i / PointCount;
		ellipse.setPoint(i, ToWindow({ center.x + radiusX * std::cos(angle), center.y + radiusY * std::sin(angle) }));
	}
	DrawFilled(ellipse);
}

sf::Vector2f SfmlCanvas::ToWindow(Point point) const
{
	return { m_origin.x + static_cast<float>(point.x), m_origin.y + static_cast<float>(point.y) };
}

void SfmlCanvas::DrawFilled(sf::Shape& shape)
{
	shape.setFillColor(m_fillColor);
	shape.setOutlineColor(m_strokeColor);
	shape.setOutlineThickness(-StrokeThickness);
	m_target.draw(shape);
}

} // namespace editor
