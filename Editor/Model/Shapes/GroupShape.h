#pragma once

#include "Model/Shapes/Shape.h"

#include <boost/signals2/connection.hpp>
#include <memory>
#include <string>
#include <vector>

namespace editor
{

class GroupShape : public Shape
{
public:
	GroupShape(std::string id, std::vector<std::unique_ptr<Shape>> shapes);

	std::string GetType() const override;
	ShapeStyle GetStyle() const override;
	void SetStyle(const ShapeStyle& style) override;
	Rect GetBounds() const override;
	void SetBounds(const Rect& bounds) override;
	bool ContainsPoint(const Point& point) const override;
	GroupShape* GetGroup() override;
	const GroupShape* GetGroup() const override;

	int GetShapeCount() const;
	const Shape& GetShapeAt(int index) const;
	Shape* FindShape(const std::string& id) const;
	std::vector<std::unique_ptr<Shape>> TakeShapes();

protected:
	void DrawGeometry(ICanvas& canvas) const override;

private:
	std::vector<std::unique_ptr<Shape>> m_shapes;
	std::vector<boost::signals2::scoped_connection> m_connections;
};

} // namespace editor
