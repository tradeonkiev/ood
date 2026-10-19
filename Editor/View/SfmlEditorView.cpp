#include "SfmlEditorView.h"

#include "Presenter/SelectionFrame.h"

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
std::array<ToolbarButton, 4> ToolbarButtons{
	ToolbarButton::Rectangle, ToolbarButton::Ellipse, ToolbarButton::Triangle, ToolbarButton::Delete
};

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

void DrawButtonIcon(sf::RenderTarget& target, ToolbarButton button, sf::Vector2f buttonPosition)
{
	constexpr float Padding = 10;
	const sf::Vector2f topLeft = buttonPosition + sf::Vector2f(Padding, Padding);
	const float size = ButtonSize - 2 * Padding;

	switch (button)
	{
	case ToolbarButton::Rectangle:
		target.draw(MakeOutlinedRect(topLeft + sf::Vector2f(0, 3), { size, size - 6 }, sf::Color(40, 40, 40), sf::Color::Transparent));
		break;
	case ToolbarButton::Ellipse: {
		sf::CircleShape ellipse(size / 2);
		ellipse.setScale({ 1, 0.75f });
		ellipse.setPosition(topLeft + sf::Vector2f(0, size / 8));
		ellipse.setFillColor(sf::Color::Transparent);
		ellipse.setOutlineColor(sf::Color(40, 40, 40));
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
		triangle.setOutlineColor(sf::Color(40, 40, 40));
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
			stroke.setFillColor(sf::Color(0, 0, 0));
			target.draw(stroke);
		}
		break;
	}
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
	case ToolbarButton::Delete:
		return std::nullopt;
	}
	return std::nullopt;
}

SfmlEditorView::SfmlEditorView(sf::RenderWindow& window)
	: m_window(window)
	, m_canvas(window, { 0, ToolbarHeight })
{
}

void SfmlEditorView::Run()
{
	while (m_window.isOpen())
	{
		while (const std::optional event = m_window.pollEvent())
		{
			HandleEvent(*event);
		}

		if (m_window.isOpen() && m_listener)
		{
			m_listener->OnPaint();
		}
	}
}

void SfmlEditorView::SetListener(IEditorViewListener* listener) { m_listener = listener; }

void SfmlEditorView::Render(const shapes::Picture& picture, const std::optional<shapes::Rect>& selectionFrame)
{
	picture.Draw(m_canvas);
	if (selectionFrame)
	{
		DrawSelection(*selectionFrame);
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
			const auto button = FindToolbarButton(pressed->position);
			if (button == ToolbarButton::Delete)
			{
				m_listener->OnDeleteSelected();
			}
			else if (button)
			{
				m_listener->OnAddShape(*ToShapeType(*button));
			}
			return;
		}
		m_mouseCaptured = true;
		m_lastMousePoint = ToPicturePoint(pressed->position);
		m_listener->OnMouseDown(m_lastMousePoint);
	}
	else if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
	{
		m_lastMousePoint = ToPicturePoint(moved->position);
		m_listener->OnMouseMove(m_lastMousePoint);
	}
	else if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>())
	{
		if (released->button == sf::Mouse::Button::Left && m_mouseCaptured)
		{
			m_mouseCaptured = false;
			m_lastMousePoint = ToPicturePoint(released->position);
			m_listener->OnMouseUp(m_lastMousePoint);
		}
	}
	else if (event.is<sf::Event::FocusLost>())
	{
		m_mouseCaptured = false;
		m_listener->OnMouseUp(m_lastMousePoint);
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
		m_listener->OnDeleteSelected();
		return;
	}

	if (!key.control)
	{
		return;
	}

	if (m_mouseCaptured)
	{
		m_mouseCaptured = false;
		m_listener->OnMouseUp(m_lastMousePoint);
	}

	switch (key.code)
	{
	case sf::Keyboard::Key::Z:
		key.shift ? m_listener->OnRedo() : m_listener->OnUndo();
		break;
	case sf::Keyboard::Key::Y:
		m_listener->OnRedo();
		break;
	default:
		break;
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
		const sf::Vector2f position = GetButtonPosition(i);
		const sf::Color fill = hovered == button ? sf::Color(220, 220, 220) : sf::Color(250, 250, 250);
		m_window.draw(MakeOutlinedRect(position, { ButtonSize, ButtonSize }, sf::Color(0, 0, 0), fill));
		DrawButtonIcon(m_window, button, position);
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
