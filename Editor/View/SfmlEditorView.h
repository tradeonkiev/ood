#pragma once

#include "View/FileDialog.h"
#include "View/SfmlCanvas.h"
#include "ViewModel/EditorViewModel.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <boost/signals2/connection.hpp>

#include <optional>
#include <string>
#include <vector>

namespace editor
{

enum class ToolbarButton
{
	Rectangle,
	Ellipse,
	Triangle,
	Delete,
};

class SfmlEditorView
{
public:
	SfmlEditorView(sf::RenderWindow& window, EditorViewModel& viewModel);
	void ProcessEvents();
	void Render();

private:
	void UpdateFromViewModel();
	void UpdateCanvasSize();
	void UpdateTitle();
	void HandleEvent(const sf::Event& event);
	void HandleKey(const sf::Event::KeyPressed& key);
	void HandleToolbarButton(ToolbarButton button);
	void SaveDocument();
	void OpenDocument();
	bool IsButtonEnabled(ToolbarButton button) const;
	void DrawSelection(const Rect& frame);
	void DrawToolbar();

	Point ToPicturePoint(sf::Vector2i position) const;
	std::optional<ToolbarButton> FindToolbarButton(sf::Vector2i position) const;

	sf::RenderWindow& m_window;
	SfmlCanvas m_canvas;
	EditorViewModel& m_viewModel;
	FileDialog m_fileDialog;
	Point m_lastMousePoint;
	bool m_mouseCaptured = false;

	std::vector<Rect> m_selectedBounds;
	bool m_canDelete = false;
	std::optional<ShapeType> m_tool;
	std::string m_title;

	boost::signals2::scoped_connection m_viewModelConnection;
};

} // namespace editor
