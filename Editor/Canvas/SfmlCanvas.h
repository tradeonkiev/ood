#pragma once

#include "Canvas/ICanvas.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>

namespace editor
{

class SfmlCanvas : public ICanvas
{
public:
	SfmlCanvas(sf::RenderTarget& target, sf::Vector2f origin);

	void Clear() override;
	void SetColor(Color color) override;
	void DrawRectangle(const Rect& bounds) override;
	void DrawPolygon(const std::vector<Point>& vertices) override;
	void DrawEllipse(Point center, double radiusX, double radiusY) override;

private:
	sf::Vector2f ToWindow(Point point) const;
	void DrawFilled(sf::Shape& shape);

	sf::RenderTarget& m_target;
	sf::Vector2f m_origin;
	sf::Color m_color = sf::Color::Black;
};

} // namespace editor
