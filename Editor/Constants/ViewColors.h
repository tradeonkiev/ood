#pragma once

#include <SFML/Graphics/Color.hpp>

namespace editor
{

inline constexpr sf::Color CanvasBackgroundColor(200, 200, 200);
inline constexpr sf::Color SheetColor(255, 255, 255);

inline constexpr sf::Color SelectionFrameColor(0, 120, 215);
inline constexpr sf::Color SelectionMarkerColor(255, 255, 255);

namespace toolbar
{

inline constexpr sf::Color BackgroundColor(240, 240, 240);
inline constexpr sf::Color ButtonColor(250, 250, 250);
inline constexpr sf::Color ButtonHoveredColor(220, 220, 220);
inline constexpr sf::Color ButtonActiveColor(190, 215, 245);
inline constexpr sf::Color ButtonBorderColor(0, 0, 0);
inline constexpr sf::Color IconColor(40, 40, 40);
inline constexpr sf::Color IconDisabledColor(180, 180, 180);
inline constexpr sf::Color EmptyFillColor(230, 230, 230);
inline constexpr sf::Color TextColor(40, 40, 40);

} // namespace toolbar

namespace timeline
{

inline constexpr sf::Color PanelColor(250, 250, 250);
inline constexpr sf::Color RulerColor(238, 238, 238);
inline constexpr sf::Color TrackColor(243, 243, 243);
inline constexpr sf::Color BorderColor(180, 180, 180);
inline constexpr sf::Color TickColor(130, 130, 130);
inline constexpr sf::Color HoveredColor(120, 120, 120);
inline constexpr sf::Color SelectedColor(0, 120, 215);
inline constexpr sf::Color SelectedNumberColor(190, 215, 245);
inline constexpr sf::Color PlayheadColor(220, 50, 50);
inline constexpr sf::Color TextColor(70, 70, 70);
inline constexpr sf::Color ScrollBarColor(160, 160, 160);

} // namespace timeline

} // namespace editor
