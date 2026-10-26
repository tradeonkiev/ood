#include "FileDialog.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <commdlg.h>

namespace editor
{

OPENFILENAMEW MakeDialogParams(void* owner, wchar_t* fileName)
{
	OPENFILENAMEW dialog{};
	dialog.lStructSize = sizeof(dialog);
	dialog.hwndOwner = static_cast<HWND>(owner);
	dialog.lpstrFilter = L"Документ (*.txt)\0*.txt\0Все файлы\0*.*\0";
	dialog.lpstrFile = fileName;
	dialog.nMaxFile = MAX_PATH;
	dialog.lpstrDefExt = L"txt";
	return dialog;
}

FileDialog::FileDialog(void* owner)
	: m_owner(owner)
{
}

std::optional<std::filesystem::path> FileDialog::AskSavePath()
{
	wchar_t fileName[MAX_PATH] = L"";
	OPENFILENAMEW dialog = MakeDialogParams(m_owner, fileName);
	dialog.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	if (!GetSaveFileNameW(&dialog))
	{
		return std::nullopt;
	}
	return std::filesystem::path(fileName);
}

std::optional<std::filesystem::path> FileDialog::AskOpenPath()
{
	wchar_t fileName[MAX_PATH] = L"";
	OPENFILENAMEW dialog = MakeDialogParams(m_owner, fileName);
	dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	if (!GetOpenFileNameW(&dialog))
	{
		return std::nullopt;
	}
	return std::filesystem::path(fileName);
}

void FileDialog::ShowError(const std::string& message)
{
	const int length = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
	std::wstring text(length, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, text.data(), length);
	MessageBoxW(static_cast<HWND>(m_owner), text.c_str(), L"Ошибка", MB_OK | MB_ICONERROR);
}

} // namespace editor
