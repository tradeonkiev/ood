#pragma once

#include "Strategies/IShapeGeometry.h"

class RectangleStrategy : public shapes::IShapeGeometry {
public:
    explicit RectangleStrategy(shapes::Rect bounds);
    RectangleStrategy(shapes::Point topLeft, double width, double height);
    void Draw(gfx::ICanvas &canvas, gfx::Color color) const override;
    void Move(int dx, int dy) override;

    void SetBounds(const shapes::Rect &bounds) override;
    shapes::Rect GetBounds() const override;
    std::string GetType() const override;
    std::string GetParameters() const override;

private:
    shapes::Rect m_bounds;
};
