#pragma once

#include "Shape/Shape.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace shapes
{
class Picture
{
public:
	using ShapeAddedSignal = boost::signals2::signal<void(const Shape& shape)>;
	using ShapeRemovedSignal = boost::signals2::signal<void(const Shape& shape)>;

	using ShapeMovedSignal = Shape::MovedSignal;
	using ShapeColorChangedSignal = Shape::ColorChangedSignal;
	using ShapeGeometryChangedSignal = Shape::GeometryChangedSignal;

	Picture() = default;
	Picture(const Picture&) = delete;
	Picture& operator=(const Picture&) = delete;

	void AddShape(std::unique_ptr<Shape> shape);
	std::unique_ptr<Shape> DeleteShape(const std::string& id);
	void Move(double dx, double dy);
	void Draw(gfx::ICanvas& canvas) const;
	Shape& GetShape(const std::string& id);
	const Shape& GetShape(const std::string& id) const;
	const Shape& GetShapeAt(std::size_t index) const;
	std::size_t GetShapeCount() const noexcept;

	boost::signals2::connection SubscribeToShapeAdded(const ShapeAddedSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeRemoved(const ShapeRemovedSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeMoved(const ShapeMovedSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeColorChanged(const ShapeColorChangedSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeGeometryChanged(const ShapeGeometryChangedSignal::slot_type& handler);

private:
	struct ShapeConnections
	{
		boost::signals2::scoped_connection moved;
		boost::signals2::scoped_connection colorChanged;
		boost::signals2::scoped_connection geometryChanged;
	};

	ShapeConnections ConnectToShape(Shape& shape);
	Shape* FindShape(const std::string& id) noexcept;
	const Shape* FindShape(const std::string& id) const noexcept;

	std::vector<std::unique_ptr<Shape>> m_shapes;
	std::unordered_map<std::string, Shape*> m_shapesById;
	std::unordered_map<std::string, ShapeConnections> m_shapeConnections;

	ShapeAddedSignal m_shapeAddedSignal;
	ShapeRemovedSignal m_shapeRemovedSignal;
	ShapeMovedSignal m_shapeMovedSignal;
	ShapeColorChangedSignal m_shapeColorChangedSignal;
	ShapeGeometryChangedSignal m_shapeGeometryChangedSignal;
};

} // namespace shapes
