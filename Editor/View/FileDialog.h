#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace editor
{

class FileDialog
{
public:
	explicit FileDialog(void* owner);

	std::optional<std::filesystem::path> AskSavePath();
	std::optional<std::filesystem::path> AskOpenPath();
	void ShowError(const std::string& message);

private:
	void* m_owner;
};

} // namespace editor
