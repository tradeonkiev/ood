#include "Picture.h"

#include "Model/Shapes/GroupShape.h"

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

	m_shapeAddedSignal(*m_shapes[index]);
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

void Picture::MoveShape(const std::string& id, int newIndex)
{
	if (newIndex < 0 || newIndex >= GetShapeCount())
	{
		throw std::out_of_range("Shape index is out of range");
	}
	const int index = GetShapeIndex(id);
	if (index == newIndex)
	{
		return;
	}

	auto shape = std::move(m_shapes[index]);
	m_shapes.erase(m_shapes.begin() + index);
	m_shapes.insert(m_shapes.begin() + newIndex, std::move(shape));

	m_shapeMovedSignal(*m_shapes[newIndex]);
}

Shape& Picture::GetShape(const std::string& id)
{
	if (Shape* shape = FindShape(id))
	{
		return *shape;
	}
	throw std::out_of_range("Shape with id " + id + " was not found");
}

const Shape& Picture::GetShape(const std::string& id) const
{
	if (const Shape* shape = FindShape(id))
	{
		return *shape;
	}
	throw std::out_of_range("Shape with id " + id + " was not found");
}

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

bool Picture::HasShape(const std::string& id) const { return m_shapeConnections.contains(id); }

std::optional<std::string> Picture::FindRootId(const std::string& id) const
{
	for (const auto& shape : m_shapes)
	{
		const GroupShape* group = shape->GetGroup();
		if (shape->GetId() == id || (group && group->FindShape(id)))
		{
			return shape->GetId();
		}
	}
	return std::nullopt;
}

Shape* Picture::FindShape(const std::string& id) const
{
	for (const auto& shape : m_shapes)
	{
		if (shape->GetId() == id)
		{
			return shape.get();
		}
		if (const GroupShape* group = shape->GetGroup())
		{
			if (Shape* found = group->FindShape(id))
			{
				return found;
			}
		}
	}
	return nullptr;
}

const Shape* Picture::FindShapeAtPoint(const Point& point, double time) const
{
	for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); it++)
	{
		if ((*it)->GetLifetime().Contains(time) && (*it)->ContainsPoint(point))
		{
			return it->get();
		}
	}
	return nullptr;
}

boost::signals2::connection Picture::SubscribeToShapeAdded(const ShapeSignal::slot_type& handler)
{
	return m_shapeAddedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeRemoved(const ShapeSignal::slot_type& handler)
{
	return m_shapeRemovedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeChanged(const ShapeSignal::slot_type& handler)
{
	return m_shapeChangedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeMoved(const ShapeSignal::slot_type& handler)
{
	return m_shapeMovedSignal.connect(handler);
}

} // namespace editor
