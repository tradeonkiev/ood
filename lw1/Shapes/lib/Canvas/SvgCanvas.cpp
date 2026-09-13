#include "SvgCanvas.h"

#include <fstream>
#include <stdexcept>
#include "Geometry\Geometry.h"

namespace gfx {
    SvgCanvas::SvgCanvas(const std::string &drawingName) : m_fileName(drawingName + ".svg") { Clear(); }

    void SvgCanvas::Clear() {
        m_content.str("");
        m_content.clear();
        m_currentX = 0;
        m_currentY = 0;
        Flush();
    }

    SvgCanvas::~SvgCanvas() = default;

    void SvgCanvas::Flush() {
        std::ofstream drawing(m_fileName);
        drawing << "<svg xmlns=\"http://www.w3.org/2000/svg\">\n" << m_content.str() << "</svg>\n";
    }

    void SvgCanvas::SetColor(Color color) { m_color = color; }

    void SvgCanvas::MoveTo(shapes::Point point) {
        m_currentX = point.x;
        m_currentY = point.y;
    }

    void SvgCanvas::LineTo(shapes::Point point) {
        m_content << "  <line x1=\"" << m_currentX << "\" y1=\"" << m_currentY << "\" x2=\"" << point.x << "\" y2=\""
                  << point.y << "\" stroke=\"" << m_color.ToHexString() << "\" />\n";
        m_currentX = point.x;
        m_currentY = point.y;
        Flush();
    }

    void SvgCanvas::DrawEllipse(shapes::Point center, double radiusX, double radiusY) {
        m_content << "  <ellipse cx=\"" << center.x << "\" cy=\"" << center.y << "\" rx=\"" << radiusX << "\" ry=\""
                  << radiusY << "\" fill=\"" << m_color.ToHexString() << "\" stroke=\"" << m_color.ToHexString()
                  << "\" />\n";
        Flush();
    }

    void SvgCanvas::DrawText(shapes::Point point, double fontSize, const std::string &text) {
        m_content << "  <text x=\"" << point.x << "\" y=\"" << point.y << "\" font-size=\"" << fontSize
                  << "\" dominant-baseline=\"hanging\" fill=\"" << m_color.ToHexString() << "\">" << text
                  << "</text>\n";
        Flush();
    }

} // namespace gfx
