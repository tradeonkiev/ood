#pragma once

#include "Model/Shapes/Shape.h"

namespace editor
{

class RectangleShape : public Shape
{
public:
	using Shape::Shape;

	std::string GetType() const override;
	bool ContainsPoint(const Point& point) const override;

protected:
	void DrawGeometry(ICanvas& canvas) const override;
};

} // namespace editor
