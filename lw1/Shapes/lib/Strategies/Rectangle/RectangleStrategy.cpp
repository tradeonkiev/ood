#include "RectangleStrategy.h"
#include "Geometry\Validation.h"

#include <sstream>

RectangleStrategy::RectangleStrategy(shapes::Rect bounds) { SetBounds(bounds); }

RectangleStrategy::RectangleStrategy(shapes::Point topLeft, double width, double height) :
    RectangleStrategy(shapes::Rect{topLeft.x, topLeft.y, width, height}) {}

void RectangleStrategy::Draw(gfx::ICanvas &canvas, gfx::Color color) const {
    canvas.SetColor(color);
    canvas.MoveTo({m_bounds.left, m_bounds.top});
    canvas.LineTo({m_bounds.Right(), m_bounds.top});
    canvas.LineTo({m_bounds.Right(), m_bounds.Bottom()});
    canvas.LineTo({m_bounds.left, m_bounds.Bottom()});
    canvas.LineTo({m_bounds.left, m_bounds.top});
    canvas.Flush();
}

void RectangleStrategy::Move(int dx, int dy) {
    m_bounds = {m_bounds.left + dx, m_bounds.top + dy, m_bounds.width, m_bounds.height};
}

shapes::Rect RectangleStrategy::GetBounds() const { return m_bounds; }

void RectangleStrategy::SetBounds(const shapes::Rect &bounds) {
    shapes::detail::ValidateBounds(bounds);
    m_bounds = bounds;
}

std::string RectangleStrategy::GetType() const { return "rectangle"; }

std::string RectangleStrategy::GetParameters() const {
    std::ostringstream output;
    output << m_bounds.left << ' ' << m_bounds.top << ' ' << m_bounds.width << ' ' << m_bounds.height;
    return output.str();
}
