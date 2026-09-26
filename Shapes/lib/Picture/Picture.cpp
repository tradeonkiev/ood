#include "Picture.h"

#include <algorithm>
#include <stdexcept>
#include <utility>


namespace shapes {
    void Picture::AddShape(std::unique_ptr<Shape> shape) {
        const std::string id = shape->GetId();
        if (m_shapesById.contains(id)) {
            throw std::invalid_argument("Shape with id:" + id + " already exists");
        }

        Shape *shapePtr = shape.get();
        m_shapes.push_back(std::move(shape));
        try {
            m_shapesById.emplace(id, shapePtr);
        } catch (...) {
            m_shapes.pop_back();
            throw;
        }

        shapePtr->RegisterObserver(*this);
        NotifyObservers(*this);
    }

    std::unique_ptr<Shape> Picture::DeleteShape(const std::string &id) {
        const auto indexedShape = m_shapesById.find(id);
        if (indexedShape == m_shapesById.end()) {
            throw std::out_of_range("Shape with id:" + id + " was not found");
        }

        Shape *shapePtr = indexedShape->second;
        const auto orderedShape = std::find_if(m_shapes.begin(), m_shapes.end(),
                                               [shapePtr](const auto &shape) { return shape.get() == shapePtr; });

        shapePtr->RemoveObserver(*this);

        auto removedShape = std::move(*orderedShape);
        m_shapes.erase(orderedShape);
        m_shapesById.erase(indexedShape);

        NotifyObservers(*this);
        return removedShape;
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

    const Shape &Picture::GetShapeAt(std::size_t index) const {
        if (index >= m_shapes.size()) {
            throw std::out_of_range("Shape index is out of range");
        }
        return *m_shapes[index];
    }

    std::size_t Picture::GetShapeCount() const noexcept { return m_shapes.size(); }

    void Picture::Move(const double dx, const double dy) {
        for (const auto &shape: m_shapes) {
            shape->Move(dx, dy);
        }
    }

    void Picture::Draw(gfx::ICanvas &canvas) const {
        canvas.Clear();
        for (const auto &shape: m_shapes) {
            shape->Draw(canvas);
        }
    }

    Shape *Picture::FindShape(const std::string &id) noexcept {
        const auto shape = m_shapesById.find(id);
        return shape != m_shapesById.end() ? shape->second : nullptr;
    }

    const Shape *Picture::FindShape(const std::string &id) const noexcept {
        const auto shape = m_shapesById.find(id);
        return shape != m_shapesById.end() ? shape->second : nullptr;
    }


    void Picture::Update(const Shape &) { NotifyObservers(*this); }

} // namespace shapes
