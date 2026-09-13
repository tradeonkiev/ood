#pragma once

#include <memory>
#include <string>
#include "Strategies/IShapeGeometry.h"

namespace shapes {
    class Shape {
    public:
        Shape(std::string id, gfx::Color color, std::unique_ptr<IShapeGeometry> strategy);

        const std::string &GetId() const noexcept;
        gfx::Color GetColor() const noexcept;
        void SetColor(gfx::Color color) noexcept;
        void Draw(gfx::ICanvas &canvas) const;
        Rect GetBounds() const;
        std::string GetType() const;
        std::string GetParameters() const;
        void SetBounds(const Rect &bounds);
        void SetStrategy(std::unique_ptr<IShapeGeometry> strategy);

    private:
        std::string m_id;
        gfx::Color m_color;
        std::unique_ptr<IShapeGeometry> m_strategy;
    };
} // namespace shapes
