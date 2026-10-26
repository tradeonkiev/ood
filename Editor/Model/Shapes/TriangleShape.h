#pragma once

#include "Model/Shapes/Shape.h"

#include <array>

namespace editor
{

// Равнобедренный треугольник, вписанный в рамку: вершина сверху посередине
class TriangleShape : public Shape
{
public:
	using Shape::Shape;

	std::string GetType() const override;
	bool ContainsPoint(const Point& point) const override;
	void Draw(ICanvas& canvas) const override;

private:
	std::array<Point, 3> GetVertices() const;
};

} // namespace editor
