#include "SfmlCanvas.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace gfx
{

SfmlCanvas::SfmlCanvas(sf::RenderTarget& target, sf::Vector2f origin)
	: m_target(target)
	, m_origin(origin)
{
}

void SfmlCanvas::Clear() { m_target.clear(sf::Color::White); }

void SfmlCanvas::SetColor(Color color) { m_color = sf::Color(color.red, color.green, color.blue); }

void SfmlCanvas::MoveTo(shapes::Point) {}
void SfmlCanvas::LineTo(shapes::Point) {}
void SfmlCanvas::Flush() const {} 
void SfmlCanvas::DrawText(shapes::Point, double, const std::string&) {}

void SfmlCanvas::DrawRectangle(const shapes::Rect& bounds)
{
	sf::RectangleShape rectangle({ static_cast<float>(bounds.width), static_cast<float>(bounds.height) });
	rectangle.setPosition(ToWindow({ bounds.left, bounds.top }));
	DrawFilled(rectangle);
}

void SfmlCanvas::DrawPolygon(const std::vector<shapes::Point>& vertices)
{
	sf::ConvexShape polygon(vertices.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		polygon.setPoint(i, ToWindow(vertices[i]));
	}
	DrawFilled(polygon);
}

void SfmlCanvas::DrawEllipse(shapes::Point center, double radiusX, double radiusY)
{
	sf::CircleShape ellipse(static_cast<float>(radiusX));
	ellipse.setPosition(ToWindow({ center.x - radiusX, center.y - radiusY }));
	ellipse.setScale({ 1, static_cast<float>(radiusY / radiusX) });
	DrawFilled(ellipse);
}

sf::Vector2f SfmlCanvas::ToWindow(shapes::Point point) const
{
	return { m_origin.x + static_cast<float>(point.x), m_origin.y + static_cast<float>(point.y) };
}

void SfmlCanvas::DrawFilled(sf::Shape& shape)
{
	shape.setFillColor(m_color);
	m_target.draw(shape);
}

} // namespace gfx
