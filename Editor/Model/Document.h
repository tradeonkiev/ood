#pragma once

#include "Model/Commands/History.h"
#include "Model/Picture.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace editor
{

class Document
{
public:
	Document();
	Document(std::vector<std::unique_ptr<Shape>> shapes, std::filesystem::path path);
	Document(const Document&) = delete;
	Document& operator=(const Document&) = delete;

	Picture& GetPicture();
	History& GetHistory();
	std::string GenerateShapeId();

	const std::optional<std::filesystem::path>& GetFilePath() const;
	void SetFilePath(std::filesystem::path path);
	std::string GetTitle() const;

private:
	Picture m_picture;
	History m_history;
	std::optional<std::filesystem::path> m_filePath;
	std::string m_untitledName;
	int m_nextShapeId = 1;
};

} // namespace editor
