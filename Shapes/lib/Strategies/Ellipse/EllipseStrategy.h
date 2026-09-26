#pragma once

#include "Geometry/Geometry.h"
#include "Strategies/IShapeGeometry.h"

class EllipseStrategy : public shapes::IShapeGeometry {
public:
    EllipseStrategy(shapes::Point center, double xRadius, double yRadius);
    void Draw(gfx::ICanvas &canvas, gfx::Color color) const override;
    void Move(int dx, int dy) override;

    void SetBounds(const shapes::Rect &bounds) override;
    shapes::Rect GetBounds() const override {
        return {m_center.x - m_xRadius, m_center.y - m_yRadius, m_xRadius * 2, m_yRadius * 2};
    }
    std::string GetType() const override;
    std::string GetParameters() const override;

private:
    shapes::Point m_center;
    double m_xRadius;
    double m_yRadius;
};
