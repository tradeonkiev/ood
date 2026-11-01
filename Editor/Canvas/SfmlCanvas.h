#pragma once

#include "Canvas/ICanvas.h"
#include "View/CanvasViewport.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>

namespace editor
{

sf::Color ToSfmlColor(Color color);

class SfmlCanvas : public ICanvas
{
public:
	SfmlCanvas(sf::RenderTarget& target, sf::Vector2f origin, const CanvasViewport& viewport);

	void SetSheetSize(double width, double height);

	void Clear() override;
	void SetFillColor(Color color) override;
	void SetStrokeColor(Color color) override;
	void DrawRectangle(const Rect& bounds) override;
	void DrawPolygon(const std::vector<Point>& vertices) override;
	void DrawEllipse(Point center, double radiusX, double radiusY) override;

private:
	sf::Vector2f ToWindow(Point point) const;
	void DrawFilled(sf::Shape& shape);

	sf::RenderTarget& m_target;
	sf::Vector2f m_origin;
	const CanvasViewport& m_viewport;
	double m_sheetWidth = 0;
	double m_sheetHeight = 0;
	sf::Color m_fillColor = sf::Color::White;
	sf::Color m_strokeColor = sf::Color::Black;
};

} // namespace editor
