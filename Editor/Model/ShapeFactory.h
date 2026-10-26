#pragma once

#include "Model/Shapes/Shape.h"

#include <memory>
#include <string>

namespace editor
{

// type: "rectangle", "ellipse" или "triangle"; для другого типа бросает std::invalid_argument
std::unique_ptr<Shape> CreateShape(const std::string& type, const std::string& id, Color color, const Rect& bounds);

} // namespace editor
