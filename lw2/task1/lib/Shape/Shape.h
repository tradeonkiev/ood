#pragma once

#include <memory>
#include <string>
#include "Observer/IObserver.h"
#include "Observer/Observable.h"
#include "Strategies/IShapeGeometry.h"
namespace shapes {
    class Shape : public observer::Observable<Shape> {
    public:
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


    private:
        Rect GetBounds() const;

        std::string m_id;
        gfx::Color m_color;
        std::unique_ptr<IShapeGeometry> m_strategy;
    };
} // namespace shapes
