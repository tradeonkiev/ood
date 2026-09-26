#include "TriangleStrategy.h"
#include "Geometry/Validation.h"

#include <sstream>

TriangleStrategy::TriangleStrategy(std::array<shapes::Point, 3> vertices) : m_vertices(vertices) {}

void TriangleStrategy::Draw(gfx::ICanvas &canvas, gfx::Color color) const {
    canvas.SetColor(color);
    canvas.MoveTo(m_vertices[0]);
    canvas.LineTo(m_vertices[1]);
    canvas.LineTo(m_vertices[2]);
    canvas.LineTo(m_vertices[0]);
    canvas.Flush();
}

void TriangleStrategy::Move(int dx, int dy) {
    for (auto &verticle: m_vertices) {
        verticle = {verticle.x + dx, verticle.y + dy};
    }
}

shapes::Rect TriangleStrategy::GetBounds() const {
    auto [minX, maxX] = std::minmax({m_vertices[0].x, m_vertices[1].x, m_vertices[2].x});
    auto [minY, maxY] = std::minmax({m_vertices[0].y, m_vertices[1].y, m_vertices[2].y});

    return {minX, minY, maxX - minX, maxY - minY};
}

void TriangleStrategy::SetBounds(const shapes::Rect &bounds) {
    shapes::detail::ValidateBounds(bounds);

    const auto oldBounds = GetBounds();

    for (auto &vertex: m_vertices) {
        vertex.x = oldBounds.width == 0 ? bounds.left
                                        : bounds.left + (vertex.x - oldBounds.left) / oldBounds.width * bounds.width;
        vertex.y = oldBounds.height == 0 ? bounds.top
                                         : bounds.top + (vertex.y - oldBounds.top) / oldBounds.height * bounds.height;
    }
}

std::string TriangleStrategy::GetType() const { return "triangle"; }

std::string TriangleStrategy::GetParameters() const {
    std::ostringstream output;
    output << m_vertices[0].x << ' ' << m_vertices[0].y << ' ' << m_vertices[1].x << ' ' << m_vertices[1].y << ' '
           << m_vertices[2].x << ' ' << m_vertices[2].y;
    return output.str();
}
