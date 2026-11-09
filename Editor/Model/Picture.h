#pragma once

#include "Canvas/ICanvas.h"
#include "Model/Shapes/Shape.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace editor
{

class Picture
{
public:
	using ShapeSignal = boost::signals2::signal<void(const Shape& shape)>;

	Picture() = default;
	Picture(const Picture&) = delete;
	Picture& operator=(const Picture&) = delete;

	void AddShape(std::unique_ptr<Shape> shape);
	void AddShape(std::unique_ptr<Shape> shape, int index);
	std::unique_ptr<Shape> DeleteShape(const std::string& id);
	void MoveShape(const std::string& id, int newIndex);

	Shape& GetShape(const std::string& id);
	const Shape& GetShape(const std::string& id) const;
	const Shape& GetShapeAt(int index) const;
	int GetShapeIndex(const std::string& id) const;
	int GetShapeCount() const;
	bool HasShape(const std::string& id) const;
	std::optional<std::string> FindRootId(const std::string& id) const;
	const Shape* FindShapeAtPoint(const Point& point, double time) const;

	boost::signals2::connection SubscribeToShapeAdded(const ShapeSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeRemoved(const ShapeSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeChanged(const ShapeSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeMoved(const ShapeSignal::slot_type& handler);

private:
	Shape* FindShape(const std::string& id) const;

	std::vector<std::unique_ptr<Shape>> m_shapes;
	std::map<std::string, boost::signals2::scoped_connection> m_shapeConnections;

	ShapeSignal m_shapeAddedSignal;
	ShapeSignal m_shapeRemovedSignal;
	ShapeSignal m_shapeChangedSignal;
	ShapeSignal m_shapeMovedSignal;
};

} // namespace editor
