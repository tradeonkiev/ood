#pragma once

#include "Model/Shapes/Shape.h"

#include <memory>
#include <string>

namespace editor
{

std::unique_ptr<Shape> CreateShape(const std::string& type, const std::string& id, const ShapeStyle& style, const Rect& bounds);

} // namespace editor
