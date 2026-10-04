#pragma once

#include <memory>
#include <string>
#include "Observer/Event.h"
#include "Strategies/IShapeGeometry.h"
namespace shapes {
    class Shape {
    public:
        using MovedEvent = observer::Event<void(const Shape &shape, int dx, int dy)>;
        using ColorChangedEvent = observer::Event<void(const Shape &shape, gfx::Color oldColor, gfx::Color newColor)>;
        using GeometryChangedEvent = observer::Event<void(const Shape &shape)>;

        Shape(std::string id, gfx::Color color, std::unique_ptr<IShapeGeometry> strategy);

        const std::string &GetId() const;
        gfx::Color GetColor() const;
        void SetColor(gfx::Color color);
        void Draw(gfx::ICanvas &canvas) const;
        void Move(int dx, int dy);
        std::string GetType() const;
        std::string GetParameters() const;
        void SetBounds(const Rect &bounds);
        void SetStrategy(std::unique_ptr<IShapeGeometry> strategy);

        observer::Subscription SubscribeToMoved(MovedEvent::Handler handler);
        observer::Subscription SubscribeToColorChanged(ColorChangedEvent::Handler handler);
        observer::Subscription SubscribeToGeometryChanged(GeometryChangedEvent::Handler handler);

    private:
        Rect GetBounds() const;

        std::string m_id;
        gfx::Color m_color;
        std::unique_ptr<IShapeGeometry> m_strategy;

        MovedEvent m_movedEvent;
        ColorChangedEvent m_colorChangedEvent;
        GeometryChangedEvent m_geometryChangedEvent;
    };
} // namespace shapes
