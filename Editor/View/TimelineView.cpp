#include "TimelineView.h"

#include "Canvas/SfmlCanvas.h"
#include "Constants/ViewColors.h"
#include "Constants/ViewConstants.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cmath>
#include <utility>

namespace editor
{

using namespace timeline;

namespace
{

void DrawRect(sf::RenderTarget& target, sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
	sf::RectangleShape rect(size);
	rect.setPosition(position);
	rect.setFillColor(color);
	target.draw(rect);
}

} // namespace

TimelineView::TimelineView(sf::RenderWindow& window, EditorViewModel& viewModel)
	: m_window(window)
	, m_viewModel(viewModel)
{
	m_hasFont = m_font.openFromFile(FontPath);
	m_viewModelConnection = m_viewModel.SubscribeToChanged([this] { Update(); });
	Update();
}

void TimelineView::Update()
{
	const Picture& picture = m_viewModel.GetPicture();
	m_rows.clear();
	for (int i = picture.GetShapeCount() - 1; i >= 0; i--)
	{
		const Shape& shape = picture.GetShapeAt(i);
		const std::string& id = shape.GetId();
		m_rows.push_back({ id, shape.GetStyle(), m_viewModel.GetShapeLifetime(id), m_viewModel.IsSelected(id) });
	}
	m_currentTime = m_viewModel.GetCurrentTime();
	m_scroll = std::clamp(m_scroll, 0.f, GetMaxScroll());
}

bool TimelineView::Contains(sf::Vector2i position) const { return GetPanelArea().contains(sf::Vector2f(position)); }

void TimelineView::MouseDown(sf::Vector2i position, bool toggleSelection)
{
	if (GetRulerArea().contains(sf::Vector2f(position)))
	{
		m_draggingTime = true;
		m_viewModel.SetCurrentTime(XToTime(static_cast<float>(position.x)));
		return;
	}

	const auto row = FindRow(position);
	if (!row)
	{
		return;
	}

	const std::string id = m_rows[*row].id;
	const auto edge = FindLifetimeEdge(m_rows[*row], static_cast<float>(position.x));
	m_viewModel.SelectShape(id, toggleSelection && !edge);
	if (edge)
	{
		m_resizingLifetime = true;
		m_viewModel.BeginLifetimeResize(id, *edge);
	}
}

void TimelineView::MouseMove(sf::Vector2i position)
{
	if (m_draggingTime)
	{
		m_viewModel.SetCurrentTime(XToTime(static_cast<float>(position.x)));
	}
	if (m_resizingLifetime)
	{
		m_viewModel.ResizeLifetime(XToTime(static_cast<float>(position.x)));
	}
}

void TimelineView::MouseUp()
{
	m_draggingTime = false;
	if (std::exchange(m_resizingLifetime, false))
	{
		m_viewModel.EndLifetimeResize();
	}
}

void TimelineView::CancelDrag()
{
	m_draggingTime = false;
	if (std::exchange(m_resizingLifetime, false))
	{
		m_viewModel.Cancel();
	}
}

void TimelineView::Scroll(float delta) { m_scroll = std::clamp(m_scroll - delta * RowHeight, 0.f, GetMaxScroll()); }

sf::FloatRect TimelineView::GetPanelArea() const
{
	const auto size = sf::Vector2f(m_window.getSize());
	return { { 0, size.y - Height }, { size.x, Height } };
}

sf::FloatRect TimelineView::GetRulerArea() const
{
	const sf::FloatRect panel = GetPanelArea();
	return { panel.position, { panel.size.x, RulerHeight } };
}

sf::FloatRect TimelineView::GetRowsArea() const
{
	const sf::FloatRect panel = GetPanelArea();
	return { { panel.position.x, panel.position.y + RulerHeight }, { panel.size.x, panel.size.y - RulerHeight } };
}

float TimelineView::GetMaxScroll() const
{
	return std::max(0.f, static_cast<float>(m_rows.size()) * RowHeight - GetRowsArea().size.y);
}

float TimelineView::TimeToX(double time) const
{
	const float left = NumberColumnWidth + TimePadding;
	const float right = static_cast<float>(m_window.getSize().x) - ScrollBarWidth - TimePadding;
	return left + static_cast<float>(time / MaxTime) * (right - left);
}

double TimelineView::XToTime(float x) const
{
	const float left = TimeToX(0);
	return MaxTime * (x - left) / (TimeToX(MaxTime) - left);
}

std::optional<int> TimelineView::FindRow(sf::Vector2i position) const
{
	const sf::FloatRect area = GetRowsArea();
	if (!area.contains(sf::Vector2f(position)))
	{
		return std::nullopt;
	}

	const int row = static_cast<int>((static_cast<float>(position.y) - area.position.y + m_scroll) / RowHeight);
	if (row < 0 || row >= static_cast<int>(m_rows.size()))
	{
		return std::nullopt;
	}
	return row;
}

std::optional<LifetimeEdge> TimelineView::FindLifetimeEdge(const Row& row, float x) const
{
	if (std::abs(x - TimeToX(row.lifetime.start)) <= EdgeGrab)
	{
		return LifetimeEdge::Start;
	}
	if (std::abs(x - TimeToX(row.lifetime.end)) <= EdgeGrab)
	{
		return LifetimeEdge::End;
	}
	return std::nullopt;
}

void TimelineView::Draw()
{
	m_scroll = std::clamp(m_scroll, 0.f, GetMaxScroll());

	const sf::FloatRect panel = GetPanelArea();
	DrawRect(m_window, panel.position, panel.size, PanelColor);

	DrawRows();
	DrawRuler();
	DrawPlayhead();
	DrawScrollBar();
}

void TimelineView::DrawRuler()
{
	const sf::FloatRect ruler = GetRulerArea();
	DrawRect(m_window, ruler.position, ruler.size, RulerColor);
	DrawRect(m_window, ruler.position, { ruler.size.x, 1 }, BorderColor);
	DrawRect(m_window, { 0, ruler.position.y + RulerHeight - 1 }, { ruler.size.x, 1 }, BorderColor);

	const int tickCount = static_cast<int>(std::lround(MaxTime / TickStep));
	for (int i = 0; i <= tickCount; i++)
	{
		const bool major = i % 2 == 0;
		const float x = TimeToX(i * TickStep);
		const float height = major ? 8.f : 4.f;
		DrawRect(m_window, { x, ruler.position.y + RulerHeight - height }, { 1, height }, TickColor);
	}
}

void TimelineView::DrawRows()
{
	const sf::FloatRect area = GetRowsArea();
	if (area.size.y <= 0)
	{
		return;
	}

	const auto windowSize = sf::Vector2f(m_window.getSize());
	sf::View view(sf::FloatRect({ 0, m_scroll }, area.size));
	view.setViewport(sf::FloatRect({ area.position.x / windowSize.x, area.position.y / windowSize.y },
		{ area.size.x / windowSize.x, area.size.y / windowSize.y }));

	const sf::View savedView = m_window.getView();
	m_window.setView(view);

	DrawRect(m_window, { NumberColumnWidth, m_scroll }, { 1, area.size.y }, BorderColor);

	const auto hovered = !m_draggingTime && !m_resizingLifetime ? FindRow(sf::Mouse::getPosition(m_window)) : std::nullopt;
	for (int i = 0; i < static_cast<int>(m_rows.size()); i++)
	{
		const float top = static_cast<float>(i) * RowHeight;
		if (top + RowHeight >= m_scroll && top <= m_scroll + area.size.y)
		{
			DrawRow(m_rows[i], top, hovered == i);
		}
	}

	DrawRect(m_window, { TimeToX(m_currentTime), m_scroll }, { 1, area.size.y }, PlayheadColor);

	m_window.setView(savedView);
}

void TimelineView::DrawRow(const Row& row, float top, bool hovered)
{
	const float trackLeft = NumberColumnWidth + 1;
	DrawRect(m_window, { trackLeft, top + 1 }, { TimeToX(MaxTime) + TimePadding - trackLeft, RowHeight - 2 },
		TrackColor);

	const float start = TimeToX(row.lifetime.start);
	sf::RectangleShape bar({ TimeToX(row.lifetime.end) - start, RowHeight - 2 * RowPadding });
	bar.setPosition({ start, top + RowPadding });
	bar.setFillColor(ToSfmlColor(row.style.fillColor));
	bar.setOutlineColor(ToSfmlColor(row.style.strokeColor));
	bar.setOutlineThickness(-1);
	m_window.draw(bar);

	if (row.selected || hovered)
	{
		bar.setFillColor(sf::Color::Transparent);
		bar.setOutlineColor(row.selected ? SelectedColor : HoveredColor);
		bar.setOutlineThickness(row.selected ? 2.f : 1.f);
		m_window.draw(bar);
	}

	DrawNumber(row, top);
}

void TimelineView::DrawNumber(const Row& row, float top)
{
	if (row.selected)
	{
		DrawRect(m_window, { 0, top }, { NumberColumnWidth, RowHeight }, SelectedNumberColor);
	}
	if (!m_hasFont)
	{
		return;
	}

	const auto digits = row.id.find_first_of("0123456789");
	const std::string number = digits == std::string::npos ? row.id : row.id.substr(digits);
	sf::Text text(m_font, number, NumberFontSize);
	text.setFillColor(TextColor);
	const sf::FloatRect bounds = text.getLocalBounds();
	text.setPosition({ (NumberColumnWidth - bounds.size.x) / 2 - bounds.position.x,
		top + (RowHeight - bounds.size.y) / 2 - bounds.position.y });
	m_window.draw(text);
}

void TimelineView::DrawPlayhead()
{
	const sf::FloatRect ruler = GetRulerArea();
	const float x = TimeToX(m_currentTime);
	DrawRect(m_window, { x, ruler.position.y }, { 1, RulerHeight }, PlayheadColor);
}

void TimelineView::DrawScrollBar()
{
	const float maxScroll = GetMaxScroll();
	if (maxScroll <= 0)
	{
		return;
	}

	const sf::FloatRect area = GetRowsArea();
	const float contentHeight = static_cast<float>(m_rows.size()) * RowHeight;
	const float thumbHeight = std::max(MinScrollThumbHeight, area.size.y * area.size.y / contentHeight);
	const float thumbTop = area.position.y + (area.size.y - thumbHeight) * m_scroll / maxScroll;
	DrawRect(m_window, { area.size.x - ScrollBarWidth - 2, thumbTop }, { ScrollBarWidth, thumbHeight },
		ScrollBarColor);
}

} // namespace editor
