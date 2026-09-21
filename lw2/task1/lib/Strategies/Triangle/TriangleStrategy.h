#pragma once

#include <algorithm>
#include <array>
#include "Strategies/IShapeGeometry.h"

class TriangleStrategy : public shapes::IShapeGeometry {
public:
    explicit TriangleStrategy(std::array<shapes::Point, 3> vertices);
    void Draw(gfx::ICanvas &canvas, gfx::Color color) const override;
    void Move(int dx, int dy) override;

    void SetBounds(const shapes::Rect &bounds) override;
    std::string GetType() const override;
    std::string GetParameters() const override;
    shapes::Rect GetBounds() const override;

private:
    std::array<shapes::Point, 3> m_vertices;
};
