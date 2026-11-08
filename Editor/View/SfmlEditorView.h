#pragma once

#include "Canvas/SfmlCanvas.h"
#include "View/CanvasViewport.h"
#include "View/ColorDialog.h"
#include "View/FileDialog.h"
#include "View/TimelineView.h"
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
	FillColor,
	StrokeColor,
};

class SfmlEditorView
{
public:
	SfmlEditorView(sf::RenderWindow& window, EditorViewModel& viewModel);
	void ProcessEvents();
	void Render();

private:
	void Update();
	void UpdateTitle();
	void HandleEvent(const sf::Event& event);
	void HandleKey(const sf::Event::KeyPressed& key);
	void HandleLayerKey(const sf::Event::KeyPressed& key);
	void Zoom(double factor);
	void MoveView(double dx, double dy);
	void OnViewportChanged();
	void HandleToolbarButton(ToolbarButton button);
	void SaveDocument();
	void OpenDocument();
	void ChooseFillColor();
	void ChooseStrokeColor();
	bool IsButtonEnabled(ToolbarButton button) const;
	void DrawSelection(const Rect& frame);
	void DrawToolbar();
	void DrawZoom();

	Point ToPicturePoint(sf::Vector2i position) const;
	std::optional<ToolbarButton> FindToolbarButton(sf::Vector2i position) const;

	sf::RenderWindow& m_window;
	CanvasViewport m_viewport;
	SfmlCanvas m_canvas;
	EditorViewModel& m_viewModel;
	FileDialog m_fileDialog;
	ColorDialog m_colorDialog;
	TimelineView m_timeline;
	Point m_lastMousePoint;
	bool m_mouseCaptured = false;
	bool m_timelineCaptured = false;

	std::vector<Rect> m_selectedBounds;
	bool m_canDelete = false;
	ShapeStyle m_style;
	std::optional<ShapeType> m_tool;
	std::string m_title;

	boost::signals2::scoped_connection m_viewModelConnection;
};

} // namespace editor
