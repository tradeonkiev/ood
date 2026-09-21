#include "Shape.h"

#include <cassert>
#include <stdexcept>
#include <utility>

namespace shapes {
    Shape::Shape(std::string id, gfx::Color color, std::unique_ptr<IShapeGeometry> strategy) :
        m_id(std::move(id)), m_color(color) {
        SetStrategy(std::move(strategy));
    }

    const std::string &Shape::GetId() const { return m_id; }

    gfx::Color Shape::GetColor() const { return m_color; }

    void Shape::SetColor(gfx::Color color) {
        m_color = color;

        Shape::NotifyChanged();
    }

    void Shape::Draw(gfx::ICanvas &canvas) const { m_strategy->Draw(canvas, m_color); }

    void Shape::Move(int dx, int dy) {
        m_strategy->Move(dx, dy);

        Shape::NotifyChanged();
    }

    Rect Shape::GetBounds() const { return m_strategy->GetBounds(); }

    std::string Shape::GetType() const { return m_strategy->GetType(); }

    std::string Shape::GetParameters() const { return m_strategy->GetParameters(); }

    void Shape::SetBounds(const Rect &bounds) {
        m_strategy->SetBounds(bounds);
        Shape::NotifyChanged();
    }

    void Shape::SetStrategy(std::unique_ptr<IShapeGeometry> strategy) {
        assert(strategy);
        m_strategy = std::move(strategy);

        Shape::NotifyChanged();
    }

    void Shape::NotifyChanged() { NotifyObservers(*this); }

} // namespace shapes
