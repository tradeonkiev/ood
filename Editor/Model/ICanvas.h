#pragma once

#include "Model/Color.h"
#include "Model/Geometry.h"

#include <vector>

namespace editor
{

class ICanvas
{
public:
	virtual ~ICanvas() = default;

	virtual void Clear() = 0;
	virtual void SetColor(Color color) = 0;
	virtual void DrawRectangle(const Rect& bounds) = 0;
	virtual void DrawEllipse(Point center, double radiusX, double radiusY) = 0;
	virtual void DrawPolygon(const std::vector<Point>& vertices) = 0;
};

} // namespace editor
