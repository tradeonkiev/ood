#pragma once

#include "Shape/Shape.h"

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Observer/Event.h"
#include "Shape/Shape.h"

namespace shapes {
    class Picture {
    public:
        using ShapeAddedEvent = observer::Event<void(const Shape &shape)>;
        using ShapeDeletedEvent = observer::Event<void(const Shape &shape)>;

        using ShapeMovedEvent = Shape::MovedEvent;
        using ShapeColorChangedEvent = Shape::ColorChangedEvent;
        using ShapeGeometryChangedEvent = Shape::GeometryChangedEvent;

        void AddShape(std::unique_ptr<Shape> shape);
        std::unique_ptr<Shape> DeleteShape(const std::string &id);
        void Move(double dx, double dy);
        void Draw(gfx::ICanvas &canvas) const;
        Shape &GetShape(const std::string &id);
        const Shape &GetShape(const std::string &id) const;
        const Shape &GetShapeAt(std::size_t index) const;
        std::size_t GetShapeCount() const noexcept;

        observer::Subscription SubscribeToShapeAdded(ShapeAddedEvent::Handler handler);
        observer::Subscription SubscribeToShapeRemoved(ShapeDeletedEvent::Handler handler);
        observer::Subscription SubscribeToShapeMoved(ShapeMovedEvent::Handler handler);
        observer::Subscription SubscribeToShapeColorChanged(ShapeColorChangedEvent::Handler handler);
        observer::Subscription SubscribeToShapeGeometryChanged(ShapeGeometryChangedEvent::Handler handler);


    private:
        struct ShapeSubscriptions {
            observer::Subscription moved;
            observer::Subscription colorChanged;
            observer::Subscription geometryChanged;
        };

        ShapeSubscriptions SubscribeToShapeEvents(Shape &shape);
        Shape *FindShape(const std::string &id) noexcept;
        const Shape *FindShape(const std::string &id) const noexcept;

        std::vector<std::unique_ptr<Shape>> m_shapes;
        std::unordered_map<std::string, Shape *> m_shapesById;
        std::unordered_map<std::string, ShapeSubscriptions> m_shapeSubscriptions;

        ShapeAddedEvent m_shapeAddedEvent;
        ShapeDeletedEvent m_shapeDeletedEvent;
        ShapeMovedEvent m_shapeMovedEvent;
        ShapeColorChangedEvent m_shapeColorChangedEvent;
        ShapeGeometryChangedEvent m_shapeGeometryChangedEvent;
    };

} // namespace shapes
