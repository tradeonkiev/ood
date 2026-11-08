#include "SfmlEditorView.h"

#include "Constants/ViewColors.h"
#include "Constants/ViewConstants.h"
#include "ViewModel/SelectionFrame.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

namespace editor
{

std::array<ToolbarButton, 6> ToolbarButtons{ ToolbarButton::Rectangle, ToolbarButton::Ellipse,
	ToolbarButton::Triangle, ToolbarButton::Delete, ToolbarButton::FillColor, ToolbarButton::StrokeColor };

sf::Vector2f GetButtonPosition(int index)
{
	return { toolbar::ButtonSpacing + static_cast<float>(index) * (toolbar::ButtonSize + toolbar::ButtonSpacing),
		(toolbar::Height - toolbar::ButtonSize) / 2 };
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

void DrawButtonIcon(sf::RenderTarget& target, ToolbarButton button, sf::Vector2f buttonPosition, bool enabled,
	const std::optional<ShapeStyle>& style)
{
	const sf::Vector2f topLeft = buttonPosition + sf::Vector2f(toolbar::IconPadding, toolbar::IconPadding);
	const float size = toolbar::ButtonSize - 2 * toolbar::IconPadding;
	const sf::Color iconColor = enabled ? toolbar::IconColor : toolbar::IconDisabledColor;

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
	case ToolbarButton::FillColor: {
		const sf::Color fill = style ? ToSfmlColor(style->fillColor) : toolbar::EmptyFillColor;
		target.draw(MakeOutlinedRect(topLeft, { size, size }, iconColor, fill));
		break;
	}
	case ToolbarButton::StrokeColor: {
		auto frame = MakeOutlinedRect(topLeft + sf::Vector2f(2, 2), { size - 4, size - 4 },
			style ? ToSfmlColor(style->strokeColor) : iconColor, sf::Color::Transparent);
		frame.setOutlineThickness(4);
		target.draw(frame);
		break;
	}
	}
}

std::optional<ShapeType> GetShapeType(ToolbarButton button)
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
	case ToolbarButton::FillColor:
	case ToolbarButton::StrokeColor:
		return std::nullopt;
	}
	return std::nullopt;
}

SfmlEditorView::SfmlEditorView(sf::RenderWindow& window, EditorViewModel& viewModel)
	: m_window(window)
	, m_canvas(window, { 0, toolbar::Height }, m_viewport)
	, m_viewModel(viewModel)
	, m_fileDialog(window.getNativeHandle())
	, m_colorDialog(window.getNativeHandle())
	, m_timeline(window, viewModel)
{
	m_viewModelConnection = m_viewModel.SubscribeToChanged([this] { Update(); });
	Update();
	m_viewModel.SetCanvasSize(SheetWidth, SheetHeight);
	m_canvas.SetSheetSize(SheetWidth, SheetHeight);
}

void SfmlEditorView::Update()
{
	m_selectedBounds = m_viewModel.GetSelectedBounds();
	m_canDelete = m_viewModel.CanDelete();
	m_style = m_viewModel.GetCurrentStyle();
	m_tool = m_viewModel.GetTool();
}

void SfmlEditorView::ProcessEvents()
{
	while (const std::optional event = m_window.pollEvent())
	{
		HandleEvent(*event);
	}
}

void SfmlEditorView::Render()
{
	if (!m_window.isOpen())
	{
		return;
	}

	UpdateTitle();
	m_viewModel.DrawPicture(m_canvas);
	for (const auto& frame : m_selectedBounds)
	{
		DrawSelection(frame);
	}
	m_timeline.Draw();
	DrawToolbar();
	m_window.display();
}

void SfmlEditorView::UpdateTitle()
{
	const std::string title = m_viewModel.GetTitle();
	if (title != m_title)
	{
		m_title = title;
		m_window.setTitle(sf::String::fromUtf8(title.begin(), title.end()));
	}
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
		if (pressed->position.y < toolbar::Height)
		{
			if (const auto button = FindToolbarButton(pressed->position); button && IsButtonEnabled(*button))
			{
				HandleToolbarButton(*button);
			}
			return;
		}
		const bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)
			|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
		if (m_timeline.Contains(pressed->position))
		{
			m_timelineCaptured = true;
			m_timeline.MouseDown(pressed->position, ctrl);
			return;
		}
		m_mouseCaptured = true;
		m_lastMousePoint = ToPicturePoint(pressed->position);
		m_viewModel.MouseDown(m_lastMousePoint, ctrl);
	}
	else if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
	{
		if (m_timelineCaptured)
		{
			m_timeline.MouseMove(moved->position);
			return;
		}
		m_lastMousePoint = ToPicturePoint(moved->position);
		m_viewModel.MouseMove(m_lastMousePoint);
	}
	else if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>())
	{
		if (released->button == sf::Mouse::Button::Left && m_timelineCaptured)
		{
			m_timelineCaptured = false;
			m_timeline.MouseUp();
		}
		else if (released->button == sf::Mouse::Button::Left && m_mouseCaptured)
		{
			m_mouseCaptured = false;
			m_lastMousePoint = ToPicturePoint(released->position);
			m_viewModel.MouseUp(m_lastMousePoint);
		}
	}
	else if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>())
	{
		if (wheel->wheel == sf::Mouse::Wheel::Vertical && m_timeline.Contains(wheel->position))
		{
			m_timeline.Scroll(wheel->delta);
		}
	}
	else if (event.is<sf::Event::FocusLost>())
	{
		m_timelineCaptured = false;
		m_timeline.CancelDrag();
		m_mouseCaptured = false;
		m_viewModel.MouseUp(m_lastMousePoint);
	}
	else if (const auto* key = event.getIf<sf::Event::KeyPressed>())
	{
		HandleKey(*key);
	}
}

void SfmlEditorView::Zoom(double zoomScale)
{
	const auto size = m_window.getSize();
	m_viewport.ZoomAt({ size.x / 2.0, (size.y - static_cast<double>(toolbar::Height + timeline::Height)) / 2 }, zoomScale);
	OnViewportChanged();
}

void SfmlEditorView::MoveView(double dx, double dy)
{
	m_viewport.MoveView(dx, dy);
	OnViewportChanged();
}

void SfmlEditorView::OnViewportChanged()
{
	m_lastMousePoint = ToPicturePoint(sf::Mouse::getPosition(m_window));
	m_viewModel.MouseMove(m_lastMousePoint);
}

void SfmlEditorView::HandleKey(const sf::Event::KeyPressed& key)
{

	if (key.code == sf::Keyboard::Key::Escape)
	{
		if (m_timelineCaptured)
		{
			m_timelineCaptured = false;
			m_timeline.CancelDrag();
			return;
		}
		m_viewModel.Cancel();
		return;
	}

	if (key.code == sf::Keyboard::Key::Delete)
	{
		m_viewModel.DeleteSelected();
		return;
	}

	if (!key.control)
	{
		HandleLayerKey(key);
		return;
	}

	switch (key.code)
	{
	case sf::Keyboard::Key::Z:
		key.shift ? m_viewModel.Redo() : m_viewModel.Undo();
		break;
	case sf::Keyboard::Key::Y:
		m_viewModel.Redo();
		break;
	case sf::Keyboard::Key::S:
		SaveDocument();
		break;
	case sf::Keyboard::Key::O:
		OpenDocument();
		break;
	case sf::Keyboard::Key::N:
		key.shift ? m_viewModel.OpenNewWindow() : m_viewModel.NewDocument();
		break;
	case sf::Keyboard::Key::Equal:
	case sf::Keyboard::Key::Add:
		Zoom(ZoomStep);
		break;
	case sf::Keyboard::Key::Hyphen:
	case sf::Keyboard::Key::Subtract:
		Zoom(1 / ZoomStep);
		break;
	case sf::Keyboard::Key::Left:
		MoveView(MoveViewStep, 0);
		break;
	case sf::Keyboard::Key::Right:
		MoveView(-MoveViewStep, 0);
		break;
	case sf::Keyboard::Key::Up:
		MoveView(0, MoveViewStep);
		break;
	case sf::Keyboard::Key::Down:
		MoveView(0, -MoveViewStep);
		break;
	default:
		break;
	}
}

void SfmlEditorView::HandleLayerKey(const sf::Event::KeyPressed& key)
{
	if (!key.alt)
	{
		return;
	}
	if (key.code == sf::Keyboard::Key::Up)
	{
		m_viewModel.MoveSelectedLayers(1);
	}
	else if (key.code == sf::Keyboard::Key::Down)
	{
		m_viewModel.MoveSelectedLayers(-1);
	}
}

void SfmlEditorView::SaveDocument()
{
	auto path = m_viewModel.GetFilePath();
	if (!path)
	{
		path = m_fileDialog.AskSavePath();
	}
	if (!path)
	{
		return;
	}

	try
	{
		m_viewModel.SaveDocument(*path);
	}
	catch (const std::exception& error)
	{
		m_fileDialog.ShowError(std::string("Не удалось сохранить документ: ") + error.what());
	}
}

void SfmlEditorView::OpenDocument()
{
	const auto path = m_fileDialog.AskOpenPath();
	if (!path)
	{
		return;
	}

	try
	{
		m_viewModel.OpenDocument(*path);
	}
	catch (const std::exception& error)
	{
		m_fileDialog.ShowError(std::string("Не удалось открыть документ: ") + error.what());
	}
}

void SfmlEditorView::ChooseFillColor()
{
	m_mouseCaptured = false;
	if (const auto color = m_colorDialog.AskColor(m_style.fillColor))
	{
		m_viewModel.SetFillColor(*color);
	}
}

void SfmlEditorView::ChooseStrokeColor()
{
	m_mouseCaptured = false;
	if (const auto color = m_colorDialog.AskColor(m_style.strokeColor))
	{
		m_viewModel.SetStrokeColor(*color);
	}
}

void SfmlEditorView::HandleToolbarButton(ToolbarButton button)
{
	switch (button)
	{
	case ToolbarButton::Delete:
		m_viewModel.DeleteSelected();
		break;
	case ToolbarButton::FillColor:
		ChooseFillColor();
		break;
	case ToolbarButton::StrokeColor:
		ChooseStrokeColor();
		break;
	default:
		m_viewModel.SelectTool(*GetShapeType(button));
		break;
	}
}

bool SfmlEditorView::IsButtonEnabled(ToolbarButton button) const
{
	switch (button)
	{
	case ToolbarButton::Delete:
		return m_canDelete;
	default:
		return true;
	}
}

void SfmlEditorView::DrawSelection(const Rect& frame)
{
	const auto toWindow = [this](const Rect& rect) {
		const Point topLeft = m_viewport.ToScreen({ rect.left, rect.top });
		const float zoom = static_cast<float>(m_viewport.GetZoom());
		return std::pair{ sf::Vector2f(static_cast<float>(topLeft.x), static_cast<float>(topLeft.y) + toolbar::Height),
			sf::Vector2f(static_cast<float>(rect.width) * zoom, static_cast<float>(rect.height) * zoom) };
	};

	const auto [framePosition, frameSize] = toWindow(frame);
	m_window.draw(MakeOutlinedRect(framePosition, frameSize, SelectionFrameColor, sf::Color::Transparent));

	for (const auto& marker : GetMarkerRects(frame))
	{
		const auto [markerPosition, markerSize] = toWindow(marker);
		m_window.draw(MakeOutlinedRect(markerPosition, markerSize, SelectionFrameColor, SelectionMarkerColor));
	}
}

void SfmlEditorView::DrawToolbar()
{
	sf::RectangleShape background({ static_cast<float>(m_window.getSize().x), toolbar::Height });
	background.setFillColor(toolbar::BackgroundColor);
	m_window.draw(background);

	const auto hovered = FindToolbarButton(sf::Mouse::getPosition(m_window));
	for (int i = 0; i < ToolbarButtons.size(); i++)
	{
		const ToolbarButton button = ToolbarButtons[i];
		const bool enabled = IsButtonEnabled(button);
		const sf::Vector2f position = GetButtonPosition(i);
		sf::Color fill = enabled && hovered == button ? toolbar::ButtonHoveredColor : toolbar::ButtonColor;
		if (m_tool && GetShapeType(button) == m_tool)
		{
			fill = toolbar::ButtonActiveColor;
		}
		m_window.draw(MakeOutlinedRect(position, { toolbar::ButtonSize, toolbar::ButtonSize }, toolbar::ButtonBorderColor, fill));
		DrawButtonIcon(m_window, button, position, enabled, m_style);
	}
	DrawZoom();
}

void SfmlEditorView::DrawZoom()
{
	static sf::Font font;
	static const bool hasFont = font.openFromFile(FontPath);
	if (!hasFont)
	{
		return;
	}

	const long percent = std::lround(m_viewport.GetZoom() * 100);
	sf::Text text(font, std::to_string(percent) + "%", toolbar::ZoomFontSize);
	text.setFillColor(toolbar::TextColor);

	const sf::FloatRect bounds = text.getLocalBounds();
	text.setPosition({ static_cast<float>(m_window.getSize().x) - bounds.size.x - bounds.position.x - 2 * toolbar::ButtonSpacing,
		(toolbar::Height - bounds.size.y) / 2 - bounds.position.y });
	m_window.draw(text);
}

Point SfmlEditorView::ToPicturePoint(sf::Vector2i position) const
{
	return m_viewport.ToPicture({ static_cast<double>(position.x), static_cast<double>(position.y) - toolbar::Height });
}

std::optional<ToolbarButton> SfmlEditorView::FindToolbarButton(sf::Vector2i position) const
{
	const sf::Vector2f point(position);
	for (int i = 0; i < ToolbarButtons.size(); i++)
	{
		const sf::FloatRect button(GetButtonPosition(i), { toolbar::ButtonSize, toolbar::ButtonSize });
		if (button.contains(point))
		{
			return ToolbarButtons[i];
		}
	}
	return std::nullopt;
}

} // namespace editor
