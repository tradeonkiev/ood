#pragma once

#include "ICanvas.h"

#include "Geometry\Geometry.h"
#include <sstream>
#include <string>
#include <vector>
namespace gfx
{

class SvgCanvas : public ICanvas
{
public:
	explicit SvgCanvas(const std::string& drawingName);
	~SvgCanvas() override;

	void Clear() override;
	void SetColor(Color color) override;
	void MoveTo(shapes::Point point) override;
	void LineTo(shapes::Point point) override;
	void DrawRectangle(const shapes::Rect& bounds) override;
	void DrawPolygon(const std::vector<shapes::Point>& vertices) override;
	void DrawEllipse(shapes::Point center, double radiusX, double radiusY) override;
	void DrawText(shapes::Point point, double fontSize, const std::string& text) override;
	void Flush() const override;

private:
	std::string m_fileName;
	std::ostringstream m_content;
	Color m_color;
	double m_currentX = 0;
	double m_currentY = 0;
};

} // namespace gfx
