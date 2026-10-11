#include "Picture.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace shapes
{
void Picture::AddShape(std::unique_ptr<Shape> shape)
{
	const std::string id = shape->GetId();
	if (m_shapesById.contains(id))
	{
		throw std::invalid_argument("Shape with id:" + id + " already exists");
	}

	Shape* shapePtr = shape.get();
	auto connections = ConnectToShape(*shapePtr);
	m_shapes.push_back(std::move(shape));
	try
	{
		m_shapesById.emplace(id, shapePtr);
		m_shapeConnections.emplace(id, std::move(connections));
	}
	catch (...)
	{
		m_shapesById.erase(id);
		m_shapes.pop_back();
		throw;
	}

	m_shapeAddedSignal(*shapePtr);
}

std::unique_ptr<Shape> Picture::DeleteShape(const std::string& id)
{
	const auto indexedShape = m_shapesById.find(id);
	if (indexedShape == m_shapesById.end())
	{
		throw std::out_of_range("Shape with id:" + id + " was not found");
	}

	Shape* shapePtr = indexedShape->second;
	const auto orderedShape = std::find_if(m_shapes.begin(), m_shapes.end(),
		[shapePtr](const auto& shape) { return shape.get() == shapePtr; });

	m_shapeConnections.erase(id);

	auto removedShape = std::move(*orderedShape);
	m_shapes.erase(orderedShape);
	m_shapesById.erase(indexedShape);

	m_shapeRemovedSignal(*shapePtr);
	return removedShape;
}

Shape& Picture::GetShape(const std::string& id)
{
	Shape* shape = FindShape(id);
	if (!shape)
	{
		throw std::out_of_range("Shape with id:" + id + " was not found");
	}

	return *shape;
}

boost::signals2::connection Picture::SubscribeToShapeAdded(const ShapeAddedSignal::slot_type& handler)
{
	return m_shapeAddedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeRemoved(const ShapeRemovedSignal::slot_type& handler)
{
	return m_shapeRemovedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeMoved(const ShapeMovedSignal::slot_type& handler)
{
	return m_shapeMovedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeColorChanged(const ShapeColorChangedSignal::slot_type& handler)
{
	return m_shapeColorChangedSignal.connect(handler);
}

boost::signals2::connection Picture::SubscribeToShapeGeometryChanged(const ShapeGeometryChangedSignal::slot_type& handler)
{
	return m_shapeGeometryChangedSignal.connect(handler);
}

Picture::ShapeConnections Picture::ConnectToShape(Shape& shape)
{
	ShapeConnections connections;

	connections.moved = shape.SubscribeToMoved(
		[this](const Shape& movedShape, int dx, int dy) { m_shapeMovedSignal(movedShape, dx, dy); });

	connections.colorChanged = shape.SubscribeToColorChanged(
		[this](const Shape& changedShape, gfx::Color oldColor, gfx::Color newColor) {
			m_shapeColorChangedSignal(changedShape, oldColor, newColor);
		});

	connections.geometryChanged = shape.SubscribeToGeometryChanged(
		[this](const Shape& changedShape) { m_shapeGeometryChangedSignal(changedShape); });

	return connections;
}

const Shape& Picture::GetShape(const std::string& id) const
{
	const Shape* shape = FindShape(id);
	if (!shape)
	{
		throw std::out_of_range("Shape with id '" + id + "' was not found");
	}

	return *shape;
}

const Shape& Picture::GetShapeAt(std::size_t index) const
{
	if (index >= m_shapes.size())
	{
		throw std::out_of_range("Shape index is out of range");
	}
	return *m_shapes[index];
}

std::size_t Picture::GetShapeCount() const noexcept { return m_shapes.size(); }

void Picture::Move(const double dx, const double dy)
{
	for (const auto& shape : m_shapes)
	{
		shape->Move(dx, dy);
	}
}

void Picture::Draw(gfx::ICanvas& canvas) const
{
	canvas.Clear();
	for (const auto& shape : m_shapes)
	{
		shape->Draw(canvas);
	}
}

Shape* Picture::FindShape(const std::string& id) noexcept
{
	const auto shape = m_shapesById.find(id);
	return shape != m_shapesById.end() ? shape->second : nullptr;
}

const Shape* Picture::FindShape(const std::string& id) const noexcept
{
	const auto shape = m_shapesById.find(id);
	return shape != m_shapesById.end() ? shape->second : nullptr;
}

} // namespace shapes
