#include "Picture.h"

#include <stdexcept>
#include <utility>

namespace shapes {
    void Picture::AddShape(std::unique_ptr<Shape> shape) {
        const std::string id = shape->GetId();
        if (FindShape(id)) {
            throw std::invalid_argument("Shape with id:" + id + " already exists");
        }

        m_shapes.push_back(std::move(shape));
    }

    void Picture::DeleteShape(const std::string &id) {
        for (auto shape = m_shapes.begin(); shape != m_shapes.end(); shape++) {
            if ((*shape)->GetId() == id) {
                m_shapes.erase(shape);
                return;
            }
        }

        throw std::out_of_range("Shape with id:" + id + " was not found");
    }

    Shape &Picture::GetShape(const std::string &id) {
        Shape *shape = FindShape(id);
        if (!shape) {
            throw std::out_of_range("Shape with id:" + id + " was not found");
        }

        return *shape;
    }

    const Shape &Picture::GetShape(const std::string &id) const {
        const Shape *shape = FindShape(id);
        if (!shape) {
            throw std::out_of_range("Shape with id '" + id + "' was not found");
        }

        return *shape;
    }

    Shape &Picture::GetShapeAt(std::size_t index) {
        if (index >= m_shapes.size()) {
            throw std::out_of_range("Shape index is out of range");
        }
        return *m_shapes[index];
    }

    const Shape &Picture::GetShapeAt(std::size_t index) const {
        if (index >= m_shapes.size()) {
            throw std::out_of_range("Shape index is out of range");
        }
        return *m_shapes[index];
    }

    std::size_t Picture::GetShapeCount() const noexcept { return m_shapes.size(); }

    void Picture::Move(const double dx, const double dy) {
        for (const auto &shape: m_shapes) {
            auto bounds = shape->GetBounds();
            bounds.left += dx;
            bounds.top += dy;
            shape->SetBounds(bounds);
        }
    }

    void Picture::Draw(gfx::ICanvas &canvas) const {
        canvas.Clear();
        for (const auto &shape: m_shapes) {
            shape->Draw(canvas);
        }
    }

    Shape *Picture::FindShape(const std::string &id) noexcept {
        for (const auto &shape: m_shapes) {
            if (shape->GetId() == id) {
                return shape.get();
            }
        }
        return nullptr;
    }

    const Shape *Picture::FindShape(const std::string &id) const noexcept {
        for (const auto &shape: m_shapes) {
            if (shape->GetId() == id) {
                return shape.get();
            }
        }
        return nullptr;
    }


} // namespace shapes
