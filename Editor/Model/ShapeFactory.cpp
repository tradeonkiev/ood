#include "ShapeFactory.h"

#include "Model/Shapes/EllipseShape.h"
#include "Model/Shapes/RectangleShape.h"
#include "Model/Shapes/TriangleShape.h"

#include <stdexcept>

namespace editor
{

std::unique_ptr<Shape> CreateShape(const std::string& type, const std::string& id, const ShapeStyle& style, const Rect& bounds)
{
	if (type == "rectangle")
	{
		return std::make_unique<RectangleShape>(id, style, bounds);
	}
	if (type == "ellipse")
	{
		return std::make_unique<EllipseShape>(id, style, bounds);
	}
	if (type == "triangle")
	{
		return std::make_unique<TriangleShape>(id, style, bounds);
	}
	throw std::invalid_argument("неподдерживаемый тип фигуры \"" + type + "\"");
}

} // namespace editor
