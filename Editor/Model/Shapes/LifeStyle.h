#pragma once

#include "Constants/EditorConstants.h"

namespace editor
{

struct LifeStyle
{
	double start = 0;
	double end = MaxTime;

	bool Contains(double time) const { return time >= start && time <= end; }
	bool operator==(const LifeStyle&) const = default;
};

} // namespace editor
