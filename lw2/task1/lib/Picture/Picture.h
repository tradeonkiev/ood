#pragma once

#include "Shape/Shape.h"

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Observer/IObserver.h"
#include "../Observer/Observable.h"

namespace shapes {
    class Picture : public observer::IObserver<Shape>, public observer::Observable<Picture> {
    public:
        void AddShape(std::unique_ptr<Shape> shape);
        void DeleteShape(const std::string &id);
        void Move(double dx, double dy);
        void Draw(gfx::ICanvas &canvas) const;
        Shape &GetShape(const std::string &id);
        const Shape &GetShape(const std::string &id) const;
        const Shape &GetShapeAt(std::size_t index) const;
        std::size_t GetShapeCount() const noexcept;

        void Update(const Shape &shape) override;

    private:
        Shape *FindShape(const std::string &id) noexcept;
        const Shape *FindShape(const std::string &id) const noexcept;
        std::vector<std::unique_ptr<Shape>> m_shapes;
        std::unordered_map<std::string, Shape *> m_shapesById;
    };

} // namespace shapes
