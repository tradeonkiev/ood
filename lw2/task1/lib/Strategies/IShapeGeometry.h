#pragma once

#include "Canvas/ICanvas.h"
#include "Geometry/Geometry.h"

#include <string>

namespace shapes {
    class IShapeGeometry {
    public:
        virtual ~IShapeGeometry() = default;
        virtual void Draw(gfx::ICanvas &canvas, gfx::Color color) const = 0;
        virtual void Move(int dx, int dy) = 0;
        virtual shapes::Rect GetBounds() const = 0;
        virtual void SetBounds(const shapes::Rect &bounds) = 0;
        virtual std::string GetType() const = 0;
        virtual std::string GetParameters() const = 0;
    };
} // namespace shapes
