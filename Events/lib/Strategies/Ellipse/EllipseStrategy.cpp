#include "EllipseStrategy.h"
#include "Geometry\Validation.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

EllipseStrategy::EllipseStrategy(shapes::Point center, double xRadius, double yRadius) :
    m_center(center), m_xRadius(std::abs(xRadius)), m_yRadius(std::abs(yRadius)) {}

void EllipseStrategy::Draw(gfx::ICanvas &canvas, gfx::Color color) const {
    canvas.SetColor(color);
    canvas.DrawEllipse(m_center, m_xRadius, m_yRadius);
    canvas.Flush();
}

void EllipseStrategy::Move(int dx, int dy) { m_center = {m_center.x + dx, m_center.y + dy}; }

void EllipseStrategy::SetBounds(const shapes::Rect &bounds) {
    shapes::detail::ValidateBounds(bounds);
    m_center = {bounds.left + bounds.width / 2, bounds.top + bounds.height / 2};
    m_xRadius = bounds.width / 2;
    m_yRadius = bounds.height / 2;
}

std::string EllipseStrategy::GetType() const { return "ellipse"; }

std::string EllipseStrategy::GetParameters() const {
    std::ostringstream output;
    output << m_center.x << ' ' << m_center.y << ' ' << m_xRadius << ' ' << m_yRadius;
    return output.str();
}
