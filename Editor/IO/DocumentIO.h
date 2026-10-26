#pragma once

#include "Model/Picture.h"

#include <filesystem>
#include <memory>
#include <vector>

namespace editor
{

class DocumentIO
{
public:
	static void Save(const Picture& picture, const std::filesystem::path& path);
	static std::vector<std::unique_ptr<Shape>> Load(const std::filesystem::path& path);
};

} // namespace editor
