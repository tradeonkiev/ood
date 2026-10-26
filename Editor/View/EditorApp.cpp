#include "EditorApp.h"

#include <SFML/System/Sleep.hpp>

#include <utility>

namespace editor
{

EditorApp::EditorWindow::EditorWindow(std::shared_ptr<Document> document, IWindowService& windows)
	: window(sf::VideoMode({ 1280, 800 }), "editor")
	, viewModel(std::move(document), windows)
	, view(window, viewModel)
{
}

void EditorApp::OpenWindow(std::shared_ptr<Document> document)
{
	m_pendingDocuments.push_back(std::move(document));
}

void EditorApp::Run()
{
	OpenPendingWindows();
	while (!m_windows.empty())
	{
		for (auto& editorWindow : m_windows)
		{
			editorWindow->view.ProcessEvents();
			editorWindow->view.Render();
		}

		RemoveClosedWindows();
		OpenPendingWindows();
		sf::sleep(sf::milliseconds(10));
	}
}

void EditorApp::OpenPendingWindows()
{
	for (auto& document : m_pendingDocuments)
	{
		m_windows.push_back(std::make_unique<EditorWindow>(std::move(document), *this));
	}
	m_pendingDocuments.clear();
}

void EditorApp::RemoveClosedWindows()
{
	std::erase_if(m_windows, [](const auto& editorWindow) { return !editorWindow->window.isOpen(); });
}

} // namespace editor
