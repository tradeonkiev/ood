#pragma once

namespace editor
{

inline constexpr unsigned WindowWidth = 1280;
inline constexpr unsigned WindowHeight = 960;
inline constexpr const char* WindowTitle = "editor";
inline constexpr unsigned FrameRateLimit = 60;
inline constexpr const char* FontPath = "C:/Windows/Fonts/segoeui.ttf";

inline constexpr double ZoomStep = 1.1;
inline constexpr double MoveViewStep = 20;

inline constexpr float ShapeStrokeThickness = 2;
inline constexpr int EllipsePointCount = 64;

namespace toolbar
{

inline constexpr float Height = 48;
inline constexpr float ButtonSize = 40;
inline constexpr float ButtonSpacing = 8;
inline constexpr float IconPadding = 10;
inline constexpr unsigned ZoomFontSize = 16;

} // namespace toolbar

namespace timeline
{

inline constexpr float Height = 220;
inline constexpr float RulerHeight = 24;
inline constexpr float RowHeight = 30;
inline constexpr float RowPadding = 4;
inline constexpr float NumberColumnWidth = 40;
inline constexpr float TimePadding = 12;
inline constexpr float EdgeGrab = 5;
inline constexpr float ScrollBarWidth = 6;
inline constexpr float MinScrollThumbHeight = 20;
inline constexpr double TickStep = 0.5;
inline constexpr unsigned NumberFontSize = 13;

} // namespace timeline

inline constexpr double SheetWidth = WindowWidth;
inline constexpr double SheetHeight = WindowHeight - toolbar::Height - timeline::Height;

} // namespace editor
