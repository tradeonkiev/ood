#pragma once

#include "Model/Shapes/Shape.h"

#include <istream>
#include <memory>
#include <string>
#include <vector>

namespace editor
{

// Читает все фигуры целиком; при первой же ошибке бросает std::runtime_error
class LoadService
{
public:
	static std::vector<std::unique_ptr<Shape>> Load(std::istream& in);

private:
	static std::unique_ptr<Shape> ParseLine(const std::string& line, const std::string& id);
	static Color ParseColor(const std::string& text);
};

} // namespace editor
