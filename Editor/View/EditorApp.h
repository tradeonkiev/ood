#pragma once

#include "Model/Document.h"
#include "View/SfmlEditorView.h"
#include "ViewModel/EditorViewModel.h"
#include "ViewModel/IWindowService.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>
#include <vector>

namespace editor
{

class EditorApp : public IWindowService
{
public:
	void OpenWindow(std::shared_ptr<Document> document) override;
	void Run();

private:
	struct EditorWindow
	{
		EditorWindow(std::shared_ptr<Document> document, IWindowService& windows);

		sf::RenderWindow window;
		EditorViewModel viewModel;
		SfmlEditorView view;
	};

	void OpenPendingWindows();
	void RemoveClosedWindows();

	std::vector<std::unique_ptr<EditorWindow>> m_windows;
	std::vector<std::shared_ptr<Document>> m_pendingDocuments;
};

} // namespace editor
