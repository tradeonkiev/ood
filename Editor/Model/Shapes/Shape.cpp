#include "Shape.h"

#include <utility>

namespace editor
{

Shape::Shape(std::string id, Color color, Rect bounds)
	: m_id(std::move(id))
	, m_color(color)
	, m_bounds(bounds)
{
}

const std::string& Shape::GetId() const { return m_id; }

Color Shape::GetColor() const { return m_color; }

Rect Shape::GetBounds() const { return m_bounds; }

void Shape::SetBounds(const Rect& bounds)
{
	m_bounds = bounds;
	m_changedSignal(*this);
}

boost::signals2::connection Shape::SubscribeToChanged(const ChangedSignal::slot_type& handler)
{
	return m_changedSignal.connect(handler);
}

} // namespace editor
