#include "Picture.h"

#include <stdexcept>
#include <utility>

namespace editor
{

void Picture::AddShape(std::unique_ptr<Shape> shape) { AddShape(std::move(shape), GetShapeCount()); }

void Picture::AddShape(std::unique_ptr<Shape> shape, int index)
{
	if (index < 0 || index > GetShapeCount())
	{
		throw std::out_of_range("Shape index is out of range");
	}
	const std::string id = shape->GetId();
	if (m_shapeConnections.contains(id))
	{
		throw std::invalid_argument("Shape with id " + id + " already exists");
	}

	m_shapeConnections[id] = shape->SubscribeToChanged([this](const Shape& changed) { m_shapeChangedSignal(changed); });
	m_shapes.insert(m_shapes.begin() + index, std::move(shape));
}

std::unique_ptr<Shape> Picture::DeleteShape(const std::string& id)
{
	const int index = GetShapeIndex(id);
	auto shape = std::move(m_shapes[index]);
	m_shapes.erase(m_shapes.begin() + index);
	m_shapeConnections.erase(id);

	m_shapeRemovedSignal(*shape);
	return shape;
}

Shape& Picture::GetShape(const std::string& id) { return *m_shapes[GetShapeIndex(id)]; }

const Shape& Picture::GetShape(const std::string& id) const { return *m_shapes[GetShapeIndex(id)]; }

const Shape& Picture::GetShapeAt(int index) const { return *m_shapes.at(index); }

int Picture::GetShapeIndex(const std::string& id) const
{
	for (int i = 0; i < GetShapeCount(); i++)
	{
		if (m_shapes[i]->GetId() == id)
		{
			return i;
		}
	}
	throw std::out_of_range("Shape with id " + id + " was not found");
}

int Picture::GetShapeCount() const { return static_cast<int>(m_shapes.size()); }

const Shape* Picture::FindShapeAtPoint(const Point& point) const
{
	for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); ++it)
	{
		if ((*it)->ContainsPoint(point))
		{
			return it->get();
		}
	}
	return nullptr;
}

void Picture::Draw(ICanvas& canvas) const
{
	canvas.Clear();
	for (const auto& shape : m_shapes)
	{
		shape->Draw(canvas);
	}
}

boost::signals2::connection Picture::SubscribeToShapeRemoved(const ShapeSignal::slot_type& handler)
{
	return m_shapeRemovedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeChanged(const ShapeSignal::slot_type& handler)
{
	return m_shapeChangedSignal.connect(handler);
}

} // namespace editor
