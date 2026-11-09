#pragma once

#include "Model/Picture.h"

#include <ostream>

namespace editor
{

class SaveService
{
public:
	static void Save(const Picture& picture, std::ostream& out);

private:
	static void SaveShape(const Shape& shape, std::ostream& out);
};

} // namespace editor
