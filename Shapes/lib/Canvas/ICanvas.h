#pragma once

#include "Color.h"
#include "Geometry/Geometry.h"

#include <string>

namespace gfx {
    class ICanvas {
    public:
        virtual ~ICanvas() = default;

        virtual void Clear() = 0;
        virtual void SetColor(Color color) = 0;
        virtual void MoveTo(shapes::Point point) = 0;
        virtual void LineTo(shapes::Point point) = 0;
        virtual void DrawEllipse(shapes::Point center, double radiusX, double radiusY) = 0;
        virtual void DrawText(shapes::Point point, double fontSize, const std::string &text) = 0;
        virtual void Flush() const = 0;
    };

} // namespace gfx
