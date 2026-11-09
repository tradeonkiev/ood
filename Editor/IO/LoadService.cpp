#include "LoadService.h"

#include "Model/ShapeFactory.h"
#include "Model/Shapes/GroupShape.h"

#include <format>
#include <sstream>
#include <stdexcept>

namespace editor
{

std::vector<std::unique_ptr<Shape>> LoadService::Load(std::istream& in)
{
	std::vector<std::string> lines;
	std::string line;
	while (std::getline(in, line))
	{
		lines.push_back(line);
	}

	std::vector<std::unique_ptr<Shape>> result;
	size_t index = 0;
	int nextId = 1;
	while (true)
	{
		while (index < lines.size() && lines[index].empty())
		{
			index++;
		}
		if (index == lines.size())
		{
			return result;
		}
		result.push_back(ParseShape(lines, index, nextId));
	}
}

std::unique_ptr<Shape> LoadService::ParseShape(const std::vector<std::string>& lines, size_t& index, int& nextId)
{
	while (index < lines.size() && lines[index].empty())
	{
		index++;
	}
	if (index == lines.size())
	{
		throw std::runtime_error("в группе не хватает фигур");
	}

	const size_t lineNumber = index + 1;
	const std::string& line = lines[index++];
	const std::string id = std::format("shape{}", nextId++);
	try
	{
		std::istringstream iss(line);
		std::string type;
		iss >> type;
		if (type == "group")
		{
			return ParseGroup(iss, lines, index, nextId, id);
		}
		return ParseLine(line, id);
	}
	catch (const std::exception& error)
	{
		const std::string message = error.what();
		if (message.starts_with("строка "))
		{
			throw;
		}
		throw std::runtime_error(std::format("строка {}: {}", lineNumber, message));
	}
}

std::unique_ptr<Shape> LoadService::ParseGroup(
	std::istringstream& iss, const std::vector<std::string>& lines, size_t& index, int& nextId, const std::string& id)
{
	int count = 0;
	if (!(iss >> count) || count < 1)
	{
		throw std::runtime_error("неверный формат, ожидается: group количество [start end]");
	}
	const LifeStyle lifetime = ParseLifetime(iss);

	std::vector<std::unique_ptr<Shape>> shapes;
	for (int i = 0; i < count; i++)
	{
		shapes.push_back(ParseShape(lines, index, nextId));
	}
	auto group = std::make_unique<GroupShape>(id, std::move(shapes));
	group->SetLifetime(lifetime);
	return group;
}

std::unique_ptr<Shape> LoadService::ParseLine(const std::string& line, const std::string& id)
{
	std::istringstream iss(line);
	std::string type;
	std::string fillColor;
	std::string strokeColor;
	Rect bounds;
	if (!(iss >> type >> fillColor >> strokeColor >> bounds.left >> bounds.top >> bounds.width >> bounds.height))
	{
		throw std::runtime_error("неверный формат, ожидается: тип #заливка #обводка left top width height [start end]");
	}

	const LifeStyle lifetime = ParseLifetime(iss);
	if (bounds.width <= 0 || bounds.height <= 0)
	{
		throw std::runtime_error("ширина и высота должны быть больше нуля");
	}

	return CreateShape(type, id, { ParseColor(fillColor), ParseColor(strokeColor) }, bounds, lifetime);
}

LifeStyle LoadService::ParseLifetime(std::istringstream& iss)
{
	LifeStyle lifetime;
	if (iss >> lifetime.start)
	{
		if (!(iss >> lifetime.end))
		{
			throw std::runtime_error("после начала времени жизни ожидается его конец");
		}
		if (lifetime.start < 0 || lifetime.end > MaxTime || lifetime.end - lifetime.start < MinDuration)
		{
			throw std::runtime_error("неверное время жизни фигуры");
		}
	}
	iss.clear();

	std::string extra;
	if (iss >> extra)
	{
		throw std::runtime_error("лишние данные в конце строки");
	}
	return lifetime;
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
