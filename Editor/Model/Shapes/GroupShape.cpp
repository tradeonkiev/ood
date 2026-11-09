#include "GroupShape.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace editor
{

GroupShape::GroupShape(std::string id, std::vector<std::unique_ptr<Shape>> shapes)
	: Shape(std::move(id), {}, {})
	, m_shapes(std::move(shapes))
{
	if (m_shapes.empty())
	{
		throw std::invalid_argument("группа не может быть пустой");
	}

	LifeStyle lifetime = m_shapes.front()->GetLifetime();
	for (const auto& shape : m_shapes)
	{
		lifetime.start = std::min(lifetime.start, shape->GetLifetime().start);
		lifetime.end = std::max(lifetime.end, shape->GetLifetime().end);
		m_connections.emplace_back(shape->SubscribeToChanged([this](const Shape&) { m_changedSignal(*this); }));
	}
	SetLifetime(lifetime);
}

std::string GroupShape::GetType() const { return "group"; }

ShapeStyle GroupShape::GetStyle() const { return m_shapes.front()->GetStyle(); }

void GroupShape::SetStyle(const ShapeStyle& style)
{
	for (const auto& shape : m_shapes)
	{
		shape->SetStyle(style);
	}
}

Rect GroupShape::GetBounds() const
{
	const Rect first = m_shapes.front()->GetBounds();
	double left = first.left;
	double top = first.top;
	double right = first.Right();
	double bottom = first.Bottom();
	for (const auto& shape : m_shapes)
	{
		const Rect bounds = shape->GetBounds();
		left = std::min(left, bounds.left);
		top = std::min(top, bounds.top);
		right = std::max(right, bounds.Right());
		bottom = std::max(bottom, bounds.Bottom());
	}
	return { left, top, right - left, bottom - top };
}

void GroupShape::SetBounds(const Rect& bounds)
{
	const Rect old = GetBounds();
	for (const auto& shape : m_shapes)
	{
		shape->SetBounds(MapRect(shape->GetBounds(), old, bounds));
	}
}

bool GroupShape::ContainsPoint(const Point& point) const
{
	return std::ranges::any_of(m_shapes, [&point](const auto& shape) { return shape->ContainsPoint(point); });
}

GroupShape* GroupShape::GetGroup() { return this; }

const GroupShape* GroupShape::GetGroup() const { return this; }

int GroupShape::GetShapeCount() const { return static_cast<int>(m_shapes.size()); }

const Shape& GroupShape::GetShapeAt(int index) const { return *m_shapes.at(index); }

Shape* GroupShape::FindShape(const std::string& id) const
{
	for (const auto& shape : m_shapes)
	{
		if (shape->GetId() == id)
		{
			return shape.get();
		}
		if (const GroupShape* group = shape->GetGroup())
		{
			if (Shape* found = group->FindShape(id))
			{
				return found;
			}
		}
	}
	return nullptr;
}

std::vector<std::unique_ptr<Shape>> GroupShape::TakeShapes()
{
	m_connections.clear();
	return std::exchange(m_shapes, {});
}

void GroupShape::DrawGeometry(ICanvas& canvas) const
{
	for (const auto& shape : m_shapes)
	{
		shape->Draw(canvas);
	}
}

} // namespace editor
