#include "ShapeFactory.h"

#include "Model/Shapes/EllipseShape.h"
#include "Model/Shapes/RectangleShape.h"
#include "Model/Shapes/TriangleShape.h"

#include <stdexcept>

namespace editor
{

std::unique_ptr<Shape> CreateShape(const std::string& type, const std::string& id, const ShapeStyle& style, const Rect& bounds,
	const LifeStyle& lifetime)
{
	std::unique_ptr<Shape> shape;
	if (type == "rectangle")
	{
		shape = std::make_unique<RectangleShape>(id, style, bounds);
	}
	else if (type == "ellipse")
	{
		shape = std::make_unique<EllipseShape>(id, style, bounds);
	}
	else if (type == "triangle")
	{
		shape = std::make_unique<TriangleShape>(id, style, bounds);
	}
	else
	{
		throw std::invalid_argument("неподдерживаемый тип фигуры \"" + type + "\"");
	}

	shape->SetLifetime(lifetime);
	return shape;
}

} // namespace editor
