#include "LineStrategy.h"
#include "Geometry/Validation.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

LineStrategy::LineStrategy(shapes::Point start, shapes::Point end) : m_start(start), m_end(end) {}

void LineStrategy::Draw(gfx::ICanvas &canvas, gfx::Color color) const {
    canvas.SetColor(color);
    canvas.MoveTo(m_start);
    canvas.LineTo(m_end);
}

shapes::Rect LineStrategy::GetBounds() const {
    return {std::min(m_start.x, m_end.x), std::min(m_start.y, m_end.y), std::abs(m_end.x - m_start.x),
            std::abs(m_end.y - m_start.y)};
}

void LineStrategy::SetBounds(const shapes::Rect &bounds) {
    shapes::detail::ValidateBounds(bounds);
    const auto oldBounds = GetBounds();

    const auto transform = [&](shapes::Point point) -> shapes::Point {
        return {
                oldBounds.width == 0 ? bounds.left
                                     : bounds.left + (point.x - oldBounds.left) / oldBounds.width * bounds.width,
                oldBounds.height == 0 ? bounds.top
                                      : bounds.top + (point.y - oldBounds.top) / oldBounds.height * bounds.height,
        };
    };
    m_start = transform(m_start);
    m_end = transform(m_end);
}

std::string LineStrategy::GetType() const { return "line"; }

std::string LineStrategy::GetParameters() const {
    std::ostringstream output;
    output << m_start.x << ' ' << m_start.y << ' ' << m_end.x << ' ' << m_end.y;
    return output.str();
}
