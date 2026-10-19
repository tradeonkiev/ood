#include "EditorPresenter.h"

#include "Model/Commands/AddShape/AddShapeCommand.h"
#include "Model/Commands/ChangeBounds/ChangeBoundsCommand.h"
#include "Model/Commands/DeleteShape/DeleteShapeCommand.h"
#include "Presenter/SelectionFrame.h"
#include "Strategies/Ellipse/EllipseStrategy.h"
#include "Strategies/Rectangle/RectangleStrategy.h"
#include "Strategies/Triangle/TriangleStrategy.h"

#include <array>
#include <memory>
#include <string>
#include <utility>

namespace editor
{

std::unique_ptr<shapes::IShapeGeometry> CreateGeometry(ShapeType type, const shapes::Rect& bounds)
{
	switch (type)
	{
	case ShapeType::Rectangle:
		return std::make_unique<RectangleStrategy>(bounds);
	case ShapeType::Ellipse:
		return std::make_unique<EllipseStrategy>(
			shapes::Point{ bounds.left + bounds.width / 2, bounds.top + bounds.height / 2 }, bounds.width / 2,
			bounds.height / 2);
	case ShapeType::Triangle:
		return std::make_unique<TriangleStrategy>(
			std::array<shapes::Point, 3>{ shapes::Point{ bounds.left + bounds.width / 2, bounds.top },
				shapes::Point{ bounds.Right(), bounds.Bottom() }, shapes::Point{ bounds.left, bounds.Bottom() } });
	}
	return nullptr;
}
EditorPresenter::EditorPresenter(shapes::Picture& picture, IEditorView& view)
	: m_picture(picture)
	, m_view(view)
{
	m_shapeRemovedConnection = m_picture.SubscribeToShapeRemoved([this](const shapes::Shape& shape) {
		if (m_state.selectedId == shape.GetId())
		{
			m_state.selectedId.reset();
			m_state.dragMode = Mode::None;
		}
	});
	m_view.SetListener(this);
}

EditorPresenter::~EditorPresenter() { m_view.SetListener(nullptr); }

void EditorPresenter::OnPaint() { m_view.Render(m_picture, GetSelectionFrame()); }

void EditorPresenter::OnMouseDown(const shapes::Point& point)
{
	FinishDrag();
	m_state.lastMousePoint = point;

	if (const auto frame = GetSelectionFrame())
	{
		if (const auto corner = HitMarker(*frame, point))
		{
			m_state.dragMode = Mode::Resize;
			m_state.resizeCorner = *corner;
			m_state.boundsBeforeDrag = *frame;
			return;
		}
	}

	const shapes::Shape* shape = m_picture.FindShapeAtPoint(point);
	if (!shape)
	{
		m_state.selectedId.reset();
		return;
	}

	m_state.selectedId = shape->GetId();
	m_state.dragMode = Mode::Move;
	m_state.boundsBeforeDrag = shape->GetBounds();
}

void EditorPresenter::OnMouseMove(const shapes::Point& point)
{
	if (m_state.dragMode == Mode::None || !m_state.selectedId)
	{
		return;
	}

	auto& shape = m_picture.GetShape(*m_state.selectedId);
	if (m_state.dragMode == Mode::Move)
	{
		shape.Move(point.x - m_state.lastMousePoint.x, point.y - m_state.lastMousePoint.y);
		m_state.lastMousePoint = point;
		return;
	}

	shape.SetBounds(ResizeFrame(m_state.boundsBeforeDrag, m_state.resizeCorner, point));
}

void EditorPresenter::OnMouseUp(const shapes::Point& point)
{
	OnMouseMove(point);
	FinishDrag();
}

void EditorPresenter::OnAddShape(ShapeType type)
{
	FinishDrag();

	const shapes::Rect bounds{ 60, 60, 200, 140 };

	auto shape = std::make_unique<shapes::Shape>(GenerateShapeId(), gfx::Color{ 111, 222, 33 }, CreateGeometry(type, bounds));
	const std::string id = shape->GetId();
	m_history.Execute(std::make_unique<AddShapeCommand>(m_picture, std::move(shape)));
	m_state.selectedId = id;
}

void EditorPresenter::OnDeleteSelected()
{
	FinishDrag();
	if (!m_state.selectedId)
	{
		return;
	}

	m_history.Execute(std::make_unique<DeleteShapeCommand>(m_picture, *m_state.selectedId));
}

void EditorPresenter::OnUndo()
{
	FinishDrag();
	m_history.Undo();
}

void EditorPresenter::OnRedo()
{
	FinishDrag();
	m_history.Redo();
}

std::optional<shapes::Rect> EditorPresenter::GetSelectionFrame() const
{
	if (!m_state.selectedId)
	{
		return std::nullopt;
	}
	return m_picture.GetShape(*m_state.selectedId).GetBounds();
}

void EditorPresenter::FinishDrag()
{
	const Mode mode = std::exchange(m_state.dragMode, Mode::None);
	if (mode == Mode::None || !m_state.selectedId)
	{
		return;
	}

	const auto& id = *m_state.selectedId;
	const shapes::Rect newBounds = m_picture.GetShape(id).GetBounds();
	if (newBounds != m_state.boundsBeforeDrag)
	{
		m_history.Execute(std::make_unique<ChangeBoundsCommand>(m_picture, id, m_state.boundsBeforeDrag, newBounds));
	}
}

std::string EditorPresenter::GenerateShapeId() { return std::format("shape{}", m_state.nextShapeId++); }

} // namespace editor
