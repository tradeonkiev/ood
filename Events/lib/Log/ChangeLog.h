#pragma once

#include <ostream>
#include "Shape/Shape.h"

namespace shapes {
    class ChangeLog {
    public:
        explicit ChangeLog(std::ostream &output) : m_output(output) {}

        void OnShapeAdded(const Shape &shape) { m_output << "Shape added: " << shape.GetId() << '\n'; }

        void OnShapeDeleted(const Shape &shape) { m_output << "Shape deleted: " << shape.GetId() << '\n'; }

        void OnShapeMoved(const Shape &shape, int dx, int dy) {
            m_output << "Shape moved: " << shape.GetId() << " by (" << dx << ", " << dy << ")\n";
        }

        void OnShapeColorChanged(const Shape &shape, gfx::Color oldColor, gfx::Color newColor) {
            m_output << "Shape color changed: " << shape.GetId() << ' ' << oldColor.ToHexString() << " -> "
                     << newColor.ToHexString() << '\n';
        }

        void OnShapeGeometryChanged(const Shape &shape) {
            m_output << "Shape geometry changed: " << shape.GetId() << ' ' << shape.GetParameters() << '\n';
        }

    private:
        std::ostream &m_output;
    };
} // namespace shapes
