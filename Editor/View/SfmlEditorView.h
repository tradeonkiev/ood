#pragma once

#include "View/SfmlCanvas.h"
#include "ViewModel/EditorViewModel.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <boost/signals2/connection.hpp>

#include <optional>

namespace editor
{

enum class ToolbarButton
{
	Rectangle,
	Ellipse,
	Triangle,
	Delete,
	Undo,
	Redo,
};

class SfmlEditorView
{
public:
	SfmlEditorView(sf::RenderWindow& window, EditorViewModel& viewModel);
	void Run();

private:
	void Render();
	void HandleEvent(const sf::Event& event);
	void HandleKey(const sf::Event::KeyPressed& key);
	void HandleToolbarButton(ToolbarButton button);
	bool IsButtonEnabled(ToolbarButton button) const;
	void DrawSelection(const shapes::Rect& frame);
	void DrawToolbar();

	shapes::Point ToPicturePoint(sf::Vector2i position) const;
	std::optional<ToolbarButton> FindToolbarButton(sf::Vector2i position) const;

	sf::RenderWindow& m_window;
	gfx::SfmlCanvas m_canvas;
	EditorViewModel& m_viewModel;
	shapes::Point m_lastMousePoint;
	bool m_mouseCaptured = false;

	std::optional<shapes::Rect> m_selectionFrame;
	bool m_canDelete = false;
	bool m_canUndo = false;
	bool m_canRedo = false;

	boost::signals2::scoped_connection m_selectionFrameBinding;
	boost::signals2::scoped_connection m_canDeleteBinding;
	boost::signals2::scoped_connection m_canUndoBinding;
	boost::signals2::scoped_connection m_canRedoBinding;
};

} // namespace editor
