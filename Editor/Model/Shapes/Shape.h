#pragma once

#include "Canvas/ICanvas.h"
#include "Geometry/Geometry.h"
#include "Model/Shapes/LifeStyle.h"
#include "Model/Shapes/ShapeStyle.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <string>

namespace editor
{

class GroupShape;

class Shape
{
public:
	using ChangedSignal = boost::signals2::signal<void(const Shape& shape)>;

	Shape(std::string id, ShapeStyle style, Rect bounds);
	virtual ~Shape() = default;

	const std::string& GetId() const;
	virtual ShapeStyle GetStyle() const;
	virtual void SetStyle(const ShapeStyle& style);
	virtual Rect GetBounds() const;
	virtual void SetBounds(const Rect& bounds);
	LifeStyle GetLifetime() const;
	void SetLifetime(const LifeStyle& lifetime);

	virtual std::string GetType() const = 0;
	virtual bool ContainsPoint(const Point& point) const = 0;
	virtual GroupShape* GetGroup();
	virtual const GroupShape* GetGroup() const;
	void Draw(ICanvas& canvas) const;

	boost::signals2::connection SubscribeToChanged(const ChangedSignal::slot_type& handler);

protected:
	virtual void DrawGeometry(ICanvas& canvas) const = 0;

	ChangedSignal m_changedSignal;

private:
	std::string m_id;
	ShapeStyle m_style;
	Rect m_bounds;
	LifeStyle m_lifetime;
};

} // namespace editor
