#pragma once

#include "ViewModel/EditorViewModel.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <boost/signals2/connection.hpp>

#include <optional>
#include <string>
#include <vector>

namespace editor
{

class TimelineView
{
public:
	TimelineView(sf::RenderWindow& window, EditorViewModel& viewModel);

	bool Contains(sf::Vector2i position) const;
	void MouseDown(sf::Vector2i position, bool toggleSelection);
	void MouseMove(sf::Vector2i position);
	void MouseUp();
	void CancelDrag();
	void Scroll(float delta);
	void Draw();

private:
	struct Row
	{
		std::string id;
		ShapeStyle style;
		LifeStyle lifetime;
		bool selected = false;
	};

	void Update();
	sf::FloatRect GetPanelArea() const;
	sf::FloatRect GetRulerArea() const;
	sf::FloatRect GetRowsArea() const;
	float GetMaxScroll() const;
	float TimeToX(double time) const;
	double XToTime(float x) const;

	std::optional<int> FindRow(sf::Vector2i position) const;
	std::optional<LifetimeEdge> FindLifetimeEdge(const Row& row, float x) const;

	void DrawRuler();
	void DrawRows();
	void DrawRow(const Row& row, float top, bool hovered);
	void DrawNumber(const Row& row, float top);
	void DrawPlayhead();
	void DrawScrollBar();

	sf::RenderWindow& m_window;
	EditorViewModel& m_viewModel;
	sf::Font m_font;
	bool m_hasFont = false;

	std::vector<Row> m_rows;
	double m_currentTime = 0;
	float m_scroll = 0;

	bool m_draggingTime = false;
	bool m_resizingLifetime = false;

	boost::signals2::scoped_connection m_viewModelConnection;
};

} // namespace editor
