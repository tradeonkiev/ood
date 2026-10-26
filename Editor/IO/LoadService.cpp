#include "LoadService.h"

#include "Model/ShapeFactory.h"

#include <format>
#include <sstream>
#include <stdexcept>

namespace editor
{

std::vector<std::unique_ptr<Shape>> LoadService::Load(std::istream& in)
{
	std::vector<std::unique_ptr<Shape>> result;

	std::string line;
	int lineNumber = 0;
	while (std::getline(in, line))
	{
		lineNumber++;
		if (line.empty())
		{
			continue;
		}

		try
		{
			result.push_back(ParseLine(line, std::format("shape{}", result.size() + 1)));
		}
		catch (const std::exception& error)
		{
			throw std::runtime_error(std::format("строка {}: {}", lineNumber, error.what()));
		}
	}

	return result;
}

std::unique_ptr<Shape> LoadService::ParseLine(const std::string& line, const std::string& id)
{
	std::istringstream iss(line);
	std::string type;
	std::string color;
	Rect bounds;
	if (!(iss >> type >> color >> bounds.left >> bounds.top >> bounds.width >> bounds.height))
	{
		throw std::runtime_error("неверный формат, ожидается: тип #цвет left top width height");
	}

	std::string extra;
	if (iss >> extra)
	{
		throw std::runtime_error("лишние данные в конце строки");
	}
	if (bounds.width <= 0 || bounds.height <= 0)
	{
		throw std::runtime_error("ширина и высота должны быть больше нуля");
	}

	return CreateShape(type, id, ParseColor(color), bounds);
}

Color LoadService::ParseColor(const std::string& text)
{
	const bool isHex = text.size() == 7 && text[0] == '#'
		&& text.find_first_not_of("0123456789abcdefABCDEF", 1) == std::string::npos;
	if (!isHex)
	{
		throw std::runtime_error("цвет \"" + text + "\" должен быть в виде #rrggbb");
	}

	const unsigned long value = std::stoul(text.substr(1), nullptr, 16);
	return { static_cast<std::uint8_t>(value >> 16), static_cast<std::uint8_t>(value >> 8),
		static_cast<std::uint8_t>(value) };
}

} // namespace editor
