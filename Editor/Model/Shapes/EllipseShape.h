#pragma once

#include "Model/Shapes/Shape.h"

namespace editor
{

class EllipseShape : public Shape
{
public:
	using Shape::Shape;

	std::string GetType() const override;
	bool ContainsPoint(const Point& point) const override;
	void Draw(ICanvas& canvas) const override;
};

} // namespace editor
