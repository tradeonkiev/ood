#pragma once

#include "Constants/EditorConstants.h"
#include "Geometry/Geometry.h"

namespace editor
{

class CanvasViewport
{
public:
	double GetZoom() const;
	Point GetOffset() const;

	Point ToPicture(const Point& screen) const;
	Point ToScreen(const Point& picture) const;

	void ZoomAt(const Point& anchorPoint, double zoomScale);
	void MoveView(double dx, double dy);

private:
	double m_zoom = 1;
	Point m_offset;
};

} // namespace editor
