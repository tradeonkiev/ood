#pragma once

#include "Shape/Shape.h"

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace shapes {
    class Picture {
    public:
        void AddShape(std::unique_ptr<Shape> shape);
        void DeleteShape(const std::string &id);
        void Move(double dx, double dy);
        void Draw(gfx::ICanvas &canvas) const;
        Shape &GetShape(const std::string &id);
        const Shape &GetShape(const std::string &id) const;
        Shape &GetShapeAt(std::size_t index);
        const Shape &GetShapeAt(std::size_t index) const;
        std::size_t GetShapeCount() const noexcept;

    private:
        Shape *FindShape(const std::string &id) noexcept;
        const Shape *FindShape(const std::string &id) const noexcept;
        std::vector<std::unique_ptr<Shape>> m_shapes;
    };

} // namespace shapes
