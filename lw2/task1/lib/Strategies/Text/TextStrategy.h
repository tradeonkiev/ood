#pragma once

#include <string>
#include "Strategies/IShapeGeometry.h"

class TextStrategy : public shapes::IShapeGeometry {
public:
    TextStrategy(shapes::Point topLeft, double fontSize, std::string text);
    void Draw(gfx::ICanvas &canvas, gfx::Color color) const override;
    void Move(int dx, int dy) override;

    void SetBounds(const shapes::Rect &bounds) override;
    shapes::Rect GetBounds() const override;
    std::string GetType() const override;
    std::string GetParameters() const override;

private:
    shapes::Point m_topLeft;
    double m_fontSize;
    std::string m_text;
};
