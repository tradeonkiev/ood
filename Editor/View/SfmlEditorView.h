#pragma once

#include "View/IEditorView.h"
#include "View/SfmlCanvas.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <optional>

namespace editor
{

enum class ToolbarButton
{
	Rectangle,
	Ellipse,
	Triangle,
	Delete,
};

class SfmlEditorView : public IEditorView
{
public:
	explicit SfmlEditorView(sf::RenderWindow& window);
	void Run();
	void SetListener(IEditorViewListener* listener) override;
	void Render(const shapes::Picture& picture, const std::optional<shapes::Rect>& selectionFrame) override;

private:
	void HandleEvent(const sf::Event& event);
	void HandleKey(const sf::Event::KeyPressed& key);
	void DrawSelection(const shapes::Rect& frame);
	void DrawToolbar();

	shapes::Point ToPicturePoint(sf::Vector2i position) const;
	std::optional<ToolbarButton> FindToolbarButton(sf::Vector2i position) const;

	sf::RenderWindow& m_window;
	gfx::SfmlCanvas m_canvas;
	IEditorViewListener* m_listener = nullptr;
	shapes::Point m_lastMousePoint;
	bool m_mouseCaptured = false;
};

} // namespace editor
