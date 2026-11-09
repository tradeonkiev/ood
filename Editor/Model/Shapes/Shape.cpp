#include "Shape.h"

#include <utility>

namespace editor
{

Shape::Shape(std::string id, ShapeStyle style, Rect bounds)
	: m_id(std::move(id))
	, m_style(style)
	, m_bounds(bounds)
{
}

const std::string& Shape::GetId() const { return m_id; }

ShapeStyle Shape::GetStyle() const { return m_style; }

void Shape::SetStyle(const ShapeStyle& style)
{
	m_style = style;
	m_changedSignal(*this);
}

Rect Shape::GetBounds() const { return m_bounds; }

void Shape::SetBounds(const Rect& bounds)
{
	m_bounds = bounds;
	m_changedSignal(*this);
}

LifeStyle Shape::GetLifetime() const { return m_lifetime; }

void Shape::SetLifetime(const LifeStyle& lifetime)
{
	m_lifetime = lifetime;
	m_changedSignal(*this);
}

void Shape::Draw(ICanvas& canvas) const
{
	const ShapeStyle style = GetStyle();
	canvas.SetFillColor(style.fillColor);
	canvas.SetStrokeColor(style.strokeColor);
	DrawGeometry(canvas);
}

GroupShape* Shape::GetGroup() { return nullptr; }

const GroupShape* Shape::GetGroup() const { return nullptr; }

boost::signals2::connection Shape::SubscribeToChanged(const ChangedSignal::slot_type& handler)
{
	return m_changedSignal.connect(handler);
}

} // namespace editor
