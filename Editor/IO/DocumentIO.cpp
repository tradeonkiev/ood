#include "DocumentIO.h"

#include "IO/LoadService.h"
#include "IO/SaveService.h"

#include <fstream>
#include <stdexcept>
#include <string>

namespace editor
{

void DocumentIO::Save(const Picture& picture, const std::filesystem::path& path)
{
	std::ofstream file(path);
	if (!file)
	{
		throw std::runtime_error("НЕ получилось открыть");
	}
	SaveService::Save(picture, file);
	if (!file)
	{
		throw std::runtime_error("Ошибка записи файла");
	}
}

std::vector<std::unique_ptr<Shape>> DocumentIO::Load(const std::filesystem::path& path)
{
	std::ifstream file(path);
	if (!file)
	{
		throw std::runtime_error("Файл недоступен");
	}
	return LoadService::Load(file);
}

} // namespace editor
