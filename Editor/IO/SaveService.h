#pragma once

#include "Model/Picture.h"

#include <ostream>

namespace editor
{

class SaveService
{
public:
	static void Save(const Picture& picture, std::ostream& out);
};

} // namespace editor
