#include "SfmlEditorView.h"

#include "ViewModel/SelectionFrame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Mouse.hpp>
#include <array>
#include <iostream>

namespace editor
{

float ToolbarHeight = 48;
float ButtonSize = 40;
float ButtonSpacing = 8;
std::array<ToolbarButton, 6> ToolbarButtons{ ToolbarButton::Rectangle, ToolbarButton::Ellipse,
	ToolbarButton::Triangle, ToolbarButton::Delete, ToolbarButton::Undo, ToolbarButton::Redo };

sf::Vector2f GetButtonPosition(int index)
{
	return { ButtonSpacing + static_cast<float>(index) * (ButtonSize + ButtonSpacing),
		(ToolbarHeight - ButtonSize) / 2 };
}

sf::RectangleShape MakeOutlinedRect(sf::Vector2f position, sf::Vector2f size, sf::Color outline, sf::Color fill)
{
	sf::RectangleShape rect(size);
	rect.setPosition(position);
	rect.setFillColor(fill);
	rect.setOutlineColor(outline);
	rect.setOutlineThickness(1);
	return rect;
}

void DrawArrowIcon(sf::RenderTarget& target, sf::Vector2f topLeft, float size, bool pointsLeft, sf::Color color)
{
	const float headWidth = size / 2.5f;
	sf::RectangleShape shaft({ size - headWidth, 2.5f });
	shaft.setPosition(topLeft + sf::Vector2f(pointsLeft ? headWidth : 0, size / 2 - 1.25f));
	shaft.setFillColor(color);
	target.draw(shaft);

	const float tipX = pointsLeft ? 0 : size;
	const float baseX = pointsLeft ? headWidth : size - headWidth;
	sf::ConvexShape head(3);
	head.setPoint(0, topLeft + sf::Vector2f(tipX, size / 2));
	head.setPoint(1, topLeft + sf::Vector2f(baseX, size / 4));
	head.setPoint(2, topLeft + sf::Vector2f(baseX, size * 3 / 4));
	head.setFillColor(color);
	target.draw(head);
}

void DrawButtonIcon(sf::RenderTarget& target, ToolbarButton button, sf::Vector2f buttonPosition, bool enabled)
{
	constexpr float Padding = 10;
	const sf::Vector2f topLeft = buttonPosition + sf::Vector2f(Padding, Padding);
	const float size = ButtonSize - 2 * Padding;
	const sf::Color iconColor = enabled ? sf::Color(40, 40, 40) : sf::Color(180, 180, 180);

	switch (button)
	{
	case ToolbarButton::Rectangle:
		target.draw(MakeOutlinedRect(topLeft + sf::Vector2f(0, 3), { size, size - 6 }, iconColor, sf::Color::Transparent));
		break;
	case ToolbarButton::Ellipse: {
		sf::CircleShape ellipse(size / 2);
		ellipse.setScale({ 1, 0.75f });
		ellipse.setPosition(topLeft + sf::Vector2f(0, size / 8));
		ellipse.setFillColor(sf::Color::Transparent);
		ellipse.setOutlineColor(iconColor);
		ellipse.setOutlineThickness(1.3f);
		target.draw(ellipse);
		break;
	}
	case ToolbarButton::Triangle: {
		sf::ConvexShape triangle(3);
		triangle.setPoint(0, topLeft + sf::Vector2f(size / 2, 0));
		triangle.setPoint(1, topLeft + sf::Vector2f(size, size));
		triangle.setPoint(2, topLeft + sf::Vector2f(0, size));
		triangle.setFillColor(sf::Color::Transparent);
		triangle.setOutlineColor(iconColor);
		triangle.setOutlineThickness(1);
		target.draw(triangle);
		break;
	}
	case ToolbarButton::Delete: {
		for (const float angle : { 45.f, -45.f })
		{
			sf::RectangleShape stroke({ size * 1.2f, 2.5f });
			stroke.setOrigin(stroke.getSize() / 2.0f);
			stroke.setPosition(topLeft + sf::Vector2f(size / 2, size / 2));
			stroke.setRotation(sf::degrees(angle));
			stroke.setFillColor(iconColor);
			target.draw(stroke);
		}
		break;
	}
	case ToolbarButton::Undo:
		DrawArrowIcon(target, topLeft, size, true, iconColor);
		break;
	case ToolbarButton::Redo:
		DrawArrowIcon(target, topLeft, size, false, iconColor);
		break;
	}
}

std::optional<ShapeType> ToShapeType(ToolbarButton button)
{
	switch (button)
	{
	case ToolbarButton::Rectangle:
		return ShapeType::Rectangle;
	case ToolbarButton::Ellipse:
		return ShapeType::Ellipse;
	case ToolbarButton::Triangle:
		return ShapeType::Triangle;
	default:
		return std::nullopt;
	}
}

SfmlEditorView::SfmlEditorView(sf::RenderWindow& window, EditorViewModel& viewModel)
	: m_window(window)
	, m_canvas(window, { 0, ToolbarHeight })
	, m_viewModel(viewModel)
{
	m_selectionFrameBinding = m_viewModel.GetSelectionFrame().Bind(
		[this](const std::optional<shapes::Rect>& frame) { m_selectionFrame = frame; });
	m_canDeleteBinding = m_viewModel.CanDelete().Bind([this](bool canDelete) { m_canDelete = canDelete; });
	m_canUndoBinding = m_viewModel.CanUndo().Bind([this](bool canUndo) { m_canUndo = canUndo; });
	m_canRedoBinding = m_viewModel.CanRedo().Bind([this](bool canRedo) { m_canRedo = canRedo; });
}

void SfmlEditorView::Run()
{
	while (m_window.isOpen())
	{
		while (const std::optional event = m_window.pollEvent())
		{
			HandleEvent(*event);
		}

		if (m_window.isOpen())
		{
			Render();
		}
	}
}

void SfmlEditorView::Render()
{
	m_viewModel.GetPicture().Draw(m_canvas);
	if (m_selectionFrame)
	{
		DrawSelection(*m_selectionFrame);
	}
	DrawToolbar();
	m_window.display();
}

void SfmlEditorView::HandleEvent(const sf::Event& event)
{
	if (event.is<sf::Event::Closed>())
	{
		m_window.close();
		return;
	}
	else if (const auto* resized = event.getIf<sf::Event::Resized>())
	{
		m_window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(resized->size))));
	}
	else if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (pressed->button != sf::Mouse::Button::Left)
		{
			return;
		}
		if (pressed->position.y < ToolbarHeight)
		{
			if (const auto button = FindToolbarButton(pressed->position); button && IsButtonEnabled(*button))
			{
				HandleToolbarButton(*button);
			}
			return;
		}
		m_mouseCaptured = true;
		m_lastMousePoint = ToPicturePoint(pressed->position);
		m_viewModel.MouseDown(m_lastMousePoint);
	}
	else if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
	{
		m_lastMousePoint = ToPicturePoint(moved->position);
		m_viewModel.MouseMove(m_lastMousePoint);
	}
	else if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>())
	{
		if (released->button == sf::Mouse::Button::Left && m_mouseCaptured)
		{
			m_mouseCaptured = false;
			m_lastMousePoint = ToPicturePoint(released->position);
			m_viewModel.MouseUp(m_lastMousePoint);
		}
	}
	else if (event.is<sf::Event::FocusLost>())
	{
		m_mouseCaptured = false;
		m_viewModel.MouseUp(m_lastMousePoint);
	}
	else if (const auto* key = event.getIf<sf::Event::KeyPressed>())
	{
		HandleKey(*key);
	}
}

void SfmlEditorView::HandleKey(const sf::Event::KeyPressed& key)
{

	if (key.code == sf::Keyboard::Key::Delete)
	{
		m_viewModel.DeleteSelected();
		return;
	}

	if (!key.control)
	{
		return;
	}

	if (m_mouseCaptured)
	{
		m_mouseCaptured = false;
		m_viewModel.MouseUp(m_lastMousePoint);
	}

	switch (key.code)
	{
	case sf::Keyboard::Key::Z:
		key.shift ? m_viewModel.Redo() : m_viewModel.Undo();
		break;
	case sf::Keyboard::Key::Y:
		m_viewModel.Redo();
		break;
	default:
		break;
	}
}

void SfmlEditorView::HandleToolbarButton(ToolbarButton button)
{
	switch (button)
	{
	case ToolbarButton::Delete:
		m_viewModel.DeleteSelected();
		break;
	case ToolbarButton::Undo:
		m_viewModel.Undo();
		break;
	case ToolbarButton::Redo:
		m_viewModel.Redo();
		break;
	default:
		m_viewModel.AddShape(*ToShapeType(button));
		break;
	}
}

bool SfmlEditorView::IsButtonEnabled(ToolbarButton button) const
{
	switch (button)
	{
	case ToolbarButton::Delete:
		return m_canDelete;
	case ToolbarButton::Undo:
		return m_canUndo;
	case ToolbarButton::Redo:
		return m_canRedo;
	default:
		return true;
	}
}

void SfmlEditorView::DrawSelection(const shapes::Rect& frame)
{
	const auto toWindow = [](const shapes::Rect& rect) {
		return std::pair{ sf::Vector2f(static_cast<float>(rect.left), static_cast<float>(rect.top) + ToolbarHeight),
			sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height)) };
	};

	const auto [framePosition, frameSize] = toWindow(frame);
	m_window.draw(MakeOutlinedRect(framePosition, frameSize, sf::Color(0, 120, 215), sf::Color::Transparent));

	for (const auto& marker : GetMarkerRects(frame))
	{
		const auto [markerPosition, markerSize] = toWindow(marker);
		m_window.draw(MakeOutlinedRect(markerPosition, markerSize, sf::Color(0, 120, 215), sf::Color::White));
	}
}

void SfmlEditorView::DrawToolbar()
{
	sf::RectangleShape background({ static_cast<float>(m_window.getSize().x), ToolbarHeight });
	background.setFillColor(sf::Color(240, 240, 240));
	m_window.draw(background);

	const auto hovered = FindToolbarButton(sf::Mouse::getPosition(m_window));
	for (int i = 0; i < ToolbarButtons.size(); i++)
	{
		const ToolbarButton button = ToolbarButtons[i];
		const bool enabled = IsButtonEnabled(button);
		const sf::Vector2f position = GetButtonPosition(i);
		const sf::Color fill = enabled && hovered == button ? sf::Color(220, 220, 220) : sf::Color(250, 250, 250);
		m_window.draw(MakeOutlinedRect(position, { ButtonSize, ButtonSize }, sf::Color(0, 0, 0), fill));
		DrawButtonIcon(m_window, button, position, enabled);
	}
}

shapes::Point SfmlEditorView::ToPicturePoint(sf::Vector2i position) const
{
	return { static_cast<double>(position.x), static_cast<double>(position.y) - ToolbarHeight };
}

std::optional<ToolbarButton> SfmlEditorView::FindToolbarButton(sf::Vector2i position) const
{
	const sf::Vector2f point(position);
	for (int i = 0; i < ToolbarButtons.size(); i++)
	{
		const sf::FloatRect button(GetButtonPosition(i), { ButtonSize, ButtonSize });
		if (button.contains(point))
		{
			return ToolbarButtons[i];
		}
	}
	return std::nullopt;
}

} // namespace editor
