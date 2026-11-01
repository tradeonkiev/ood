#pragma once

#include "Canvas/Color.h"

namespace editor
{

struct ShapeStyle
{
	Color fillColor;
	Color strokeColor;

	bool operator==(const ShapeStyle&) const = default;
};

} // namespace editor
