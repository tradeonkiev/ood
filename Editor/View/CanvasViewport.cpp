#include "CanvasViewport.h"

#include <algorithm>

namespace editor
{

double CanvasViewport::GetZoom() const { return m_zoom; }

Point CanvasViewport::GetOffset() const { return m_offset; }

Point CanvasViewport::ToPicture(const Point& screen) const
{
	return { screen.x / m_zoom + m_offset.x, screen.y / m_zoom + m_offset.y };
}

Point CanvasViewport::ToScreen(const Point& picture) const
{
	return { (picture.x - m_offset.x) * m_zoom, (picture.y - m_offset.y) * m_zoom };
}

void CanvasViewport::ZoomAt(const Point& anchor, double zoomScale)
{
	const Point fixed = ToPicture(anchor);
	m_zoom = std::clamp(m_zoom * zoomScale, MinZoom, MaxZoom);
	m_offset = { fixed.x - anchor.x / m_zoom, fixed.y - anchor.y / m_zoom };
}

void CanvasViewport::MoveView(double dx, double dy)
{
	m_offset.x += dx / m_zoom;
	m_offset.y += dy / m_zoom;
}

} // namespace editor
