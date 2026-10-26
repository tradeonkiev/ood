#pragma once

#include "Model/Color.h"
#include "Model/Geometry.h"
#include "Model/ICanvas.h"

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <string>

namespace editor
{

class Shape
{
public:
	using ChangedSignal = boost::signals2::signal<void(const Shape& shape)>;

	Shape(std::string id, Color color, Rect bounds);
	virtual ~Shape() = default;

	const std::string& GetId() const;
	Color GetColor() const;
	Rect GetBounds() const;
	void SetBounds(const Rect& bounds);

	virtual std::string GetType() const = 0;
	virtual bool ContainsPoint(const Point& point) const = 0;
	virtual void Draw(ICanvas& canvas) const = 0;

	boost::signals2::connection SubscribeToChanged(const ChangedSignal::slot_type& handler);

private:
	std::string m_id;
	Color m_color;
	Rect m_bounds;
	ChangedSignal m_changedSignal;
};

} // namespace editor
