#pragma once

#include "Model/ICanvas.h"
#include "Model/Shapes/Shape.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <map>
#include <memory>
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

	Shape& GetShape(const std::string& id);
	const Shape& GetShape(const std::string& id) const;
	const Shape& GetShapeAt(int index) const;
	int GetShapeIndex(const std::string& id) const;
	int GetShapeCount() const;
	const Shape* FindShapeAtPoint(const Point& point) const;

	void Draw(ICanvas& canvas) const;

	boost::signals2::connection SubscribeToShapeRemoved(const ShapeSignal::slot_type& handler);
	boost::signals2::connection SubscribeToShapeChanged(const ShapeSignal::slot_type& handler);

private:
	std::vector<std::unique_ptr<Shape>> m_shapes;
	std::map<std::string, boost::signals2::scoped_connection> m_shapeConnections;

	ShapeSignal m_shapeRemovedSignal;
	ShapeSignal m_shapeChangedSignal;
};

} // namespace editor
