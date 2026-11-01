#include "SfmlCanvas.h"

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <cmath>
#include <numbers>

namespace editor
{

constexpr float StrokeThickness = 2;

SfmlCanvas::SfmlCanvas(sf::RenderTarget& target, sf::Vector2f origin, const CanvasViewport& viewport)
	: m_target(target)
	, m_origin(origin)
	, m_viewport(viewport)
{
}

void SfmlCanvas::SetSheetSize(double width, double height)
{
	m_sheetWidth = width;
	m_sheetHeight = height;
}

void SfmlCanvas::Clear()
{
	m_target.clear(sf::Color(200, 200, 200));

	const float zoom = static_cast<float>(m_viewport.GetZoom());
	sf::RectangleShape sheet(
		{ static_cast<float>(m_sheetWidth) * zoom, static_cast<float>(m_sheetHeight) * zoom });
	sheet.setPosition(ToWindow({ 0, 0 }));
	sheet.setFillColor(sf::Color::White);
	m_target.draw(sheet);
}

sf::Color ToSfmlColor(Color color) { return { color.red, color.green, color.blue }; }

void SfmlCanvas::SetFillColor(Color color) { m_fillColor = ToSfmlColor(color); }

void SfmlCanvas::SetStrokeColor(Color color) { m_strokeColor = ToSfmlColor(color); }

void SfmlCanvas::DrawRectangle(const Rect& bounds)
{
	const float zoom = static_cast<float>(m_viewport.GetZoom());
	sf::RectangleShape rectangle(
		{ static_cast<float>(bounds.width) * zoom, static_cast<float>(bounds.height) * zoom });
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
	const Point screen = m_viewport.ToScreen(point);
	return { m_origin.x + static_cast<float>(screen.x), m_origin.y + static_cast<float>(screen.y) };
}

void SfmlCanvas::DrawFilled(sf::Shape& shape)
{
	shape.setFillColor(m_fillColor);
	shape.setOutlineColor(m_strokeColor);
	shape.setOutlineThickness(-StrokeThickness * static_cast<float>(m_viewport.GetZoom()));
	m_target.draw(shape);
}

} // namespace editor
