#pragma once

#include "Model/Shapes/Shape.h"

#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace editor
{

class LoadService
{
public:
	static std::vector<std::unique_ptr<Shape>> Load(std::istream& in);

private:
	static std::unique_ptr<Shape> ParseShape(const std::vector<std::string>& lines, size_t& index, int& nextId);
	static std::unique_ptr<Shape> ParseGroup(
		std::istringstream& iss, const std::vector<std::string>& lines, size_t& index, int& nextId, const std::string& id);
	static std::unique_ptr<Shape> ParseLine(const std::string& line, const std::string& id);
	static LifeStyle ParseLifetime(std::istringstream& iss);
	static Color ParseColor(const std::string& text);
};

} // namespace editor
