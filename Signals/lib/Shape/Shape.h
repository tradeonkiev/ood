#pragma once

#include "Strategies/IShapeGeometry.h"
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>
#include <string>

namespace shapes
{
class Shape
{
public:
	using MovedSignal = boost::signals2::signal<void(const Shape& shape, double dx, double dy)>;
	using ColorChangedSignal = boost::signals2::signal<void(const Shape& shape, gfx::Color oldColor, gfx::Color newColor)>;
	using GeometryChangedSignal = boost::signals2::signal<void(const Shape& shape)>;

	Shape(std::string id, gfx::Color color, std::unique_ptr<IShapeGeometry> strategy);

	const std::string& GetId() const;
	gfx::Color GetColor() const;
	void SetColor(gfx::Color color);
	void Draw(gfx::ICanvas& canvas) const;
	void Move(double dx, double dy);
	bool ContainsPoint(const shapes::Point& point) const;
	std::string GetType() const;
	std::string GetParameters() const;
	Rect GetBounds() const;
	void SetBounds(const Rect& bounds);
	void SetStrategy(std::unique_ptr<IShapeGeometry> strategy);

	boost::signals2::connection SubscribeToMoved(const MovedSignal::slot_type& handler);
	boost::signals2::connection SubscribeToColorChanged(const ColorChangedSignal::slot_type& handler);
	boost::signals2::connection SubscribeToGeometryChanged(const GeometryChangedSignal::slot_type& handler);

private:
	std::string m_id;
	gfx::Color m_color;
	std::unique_ptr<IShapeGeometry> m_strategy;

	MovedSignal m_movedSignal;
	ColorChangedSignal m_colorChangedSignal;
	GeometryChangedSignal m_geometryChangedSignal;
};
} // namespace shapes
