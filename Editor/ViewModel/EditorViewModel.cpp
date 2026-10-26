#include "EditorViewModel.h"

#include "Model/Commands/AddShape/AddShapeCommand.h"
#include "Model/Commands/ChangeBounds/ChangeBoundsCommand.h"
#include "Model/Commands/DeleteShape/DeleteShapeCommand.h"
#include "Strategies/Ellipse/EllipseStrategy.h"
#include "Strategies/Rectangle/RectangleStrategy.h"
#include "Strategies/Triangle/TriangleStrategy.h"

#include <array>
#include <format>
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

EditorViewModel::EditorViewModel(shapes::Picture& picture)
	: m_picture(picture)
{
	m_shapeRemovedConnection = m_picture.SubscribeToShapeRemoved([this](const shapes::Shape& shape) {
		if (m_state.selectedId == shape.GetId())
		{
			m_state.dragMode = Mode::None;
			Select(std::nullopt);
		}
	});
	m_shapeMovedConnection
		= m_picture.SubscribeToShapeMoved([this](const shapes::Shape&, double, double) { UpdateSelectionFrame(); });
	m_shapeGeometryChangedConnection
		= m_picture.SubscribeToShapeGeometryChanged([this](const shapes::Shape&) { UpdateSelectionFrame(); });
}

const shapes::Picture& EditorViewModel::GetPicture() const { return m_picture; }

const ObservableValue<std::optional<shapes::Rect>>& EditorViewModel::GetSelectionFrame() const
{
	return m_selectionFrame;
}

const ObservableValue<bool>& EditorViewModel::CanDelete() const { return m_canDelete; }

const ObservableValue<bool>& EditorViewModel::CanUndo() const { return m_canUndo; }

const ObservableValue<bool>& EditorViewModel::CanRedo() const { return m_canRedo; }

void EditorViewModel::MouseDown(const shapes::Point& point)
{
	FinishDrag();
	m_state.lastMousePoint = point;

	if (const auto& frame = m_selectionFrame.Get())
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
		Select(std::nullopt);
		return;
	}

	Select(shape->GetId());
	m_state.dragMode = Mode::Move;
	m_state.boundsBeforeDrag = shape->GetBounds();
}

void EditorViewModel::MouseMove(const shapes::Point& point)
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

void EditorViewModel::MouseUp(const shapes::Point& point)
{
	MouseMove(point);
	FinishDrag();
}

void EditorViewModel::AddShape(ShapeType type)
{
	FinishDrag();

	const shapes::Rect bounds{ 60, 60, 200, 140 };

	auto shape = std::make_unique<shapes::Shape>(GenerateShapeId(), gfx::Color{ 111, 222, 33 }, CreateGeometry(type, bounds));
	const std::string id = shape->GetId();
	ExecuteInHistory(std::make_unique<AddShapeCommand>(m_picture, std::move(shape)));
	Select(id);
}

void EditorViewModel::DeleteSelected()
{
	FinishDrag();
	if (!m_state.selectedId)
	{
		return;
	}

	ExecuteInHistory(std::make_unique<DeleteShapeCommand>(m_picture, *m_state.selectedId));
}

void EditorViewModel::Undo()
{
	FinishDrag();
	m_history.Undo();
	UpdateHistoryState();
}

void EditorViewModel::Redo()
{
	FinishDrag();
	m_history.Redo();
	UpdateHistoryState();
}

void EditorViewModel::Select(std::optional<std::string> id)
{
	m_state.selectedId = std::move(id);
	m_canDelete.Set(m_state.selectedId.has_value());
	UpdateSelectionFrame();
}

void EditorViewModel::UpdateSelectionFrame()
{
	if (!m_state.selectedId)
	{
		m_selectionFrame.Set(std::nullopt);
		return;
	}
	m_selectionFrame.Set(m_picture.GetShape(*m_state.selectedId).GetBounds());
}

void EditorViewModel::ExecuteInHistory(std::unique_ptr<ICommand> command)
{
	m_history.Execute(std::move(command));
	UpdateHistoryState();
}

void EditorViewModel::UpdateHistoryState()
{
	m_canUndo.Set(m_history.CanUndo());
	m_canRedo.Set(m_history.CanRedo());
}

void EditorViewModel::FinishDrag()
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
		ExecuteInHistory(std::make_unique<ChangeBoundsCommand>(m_picture, id, m_state.boundsBeforeDrag, newBounds));
	}
}

std::string EditorViewModel::GenerateShapeId() { return std::format("shape{}", m_state.nextShapeId++); }

} // namespace editor
