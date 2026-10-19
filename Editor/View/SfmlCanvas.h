#pragma once

#include "Canvas/ICanvas.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>

namespace gfx
{

class SfmlCanvas : public ICanvas
{
public:
	SfmlCanvas(sf::RenderTarget& target, sf::Vector2f origin);

	void Clear() override;
	void SetColor(Color color) override;
	void MoveTo(shapes::Point point) override;
	void LineTo(shapes::Point point) override;
	void DrawRectangle(const shapes::Rect& bounds) override;
	void DrawPolygon(const std::vector<shapes::Point>& vertices) override;
	void DrawEllipse(shapes::Point center, double radiusX, double radiusY) override;
	void DrawText(shapes::Point point, double fontSize, const std::string& text) override;
	void Flush() const override;

private:
	sf::Vector2f ToWindow(shapes::Point point) const;
	void DrawFilled(sf::Shape& shape);

	sf::RenderTarget& m_target;
	sf::Vector2f m_origin;
	sf::Color m_color = sf::Color::Black;
};

} // namespace gfx
