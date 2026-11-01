#pragma once

#include "Canvas/ICanvas.h"
#include "Geometry/Geometry.h"
#include "Model/Shapes/ShapeStyle.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <string>

namespace editor
{

class Shape
{
public:
	using ChangedSignal = boost::signals2::signal<void(const Shape& shape)>;

	Shape(std::string id, ShapeStyle style, Rect bounds);
	virtual ~Shape() = default;

	const std::string& GetId() const;
	ShapeStyle GetStyle() const;
	void SetStyle(const ShapeStyle& style);
	Rect GetBounds() const;
	void SetBounds(const Rect& bounds);

	virtual std::string GetType() const = 0;
	virtual bool ContainsPoint(const Point& point) const = 0;
	void Draw(ICanvas& canvas) const;

	boost::signals2::connection SubscribeToChanged(const ChangedSignal::slot_type& handler);

protected:
	virtual void DrawGeometry(ICanvas& canvas) const = 0;

private:
	std::string m_id;
	ShapeStyle m_style;
	Rect m_bounds;
	ChangedSignal m_changedSignal;
};

} // namespace editor
