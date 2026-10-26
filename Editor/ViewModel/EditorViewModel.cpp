#include "EditorViewModel.h"

#include "Model/Commands/AddShape/AddShapeCommand.h"
#include "Model/Commands/ChangeBounds/ChangeBoundsCommand.h"
#include "Model/Commands/DeleteShape/DeleteShapeCommand.h"
#include "Model/Commands/Group/GroupCommand.h"
#include "IO/DocumentIO.h"
#include "Model/ShapeFactory.h"

#include <algorithm>
#include <format>
#include <memory>
#include <string>
#include <utility>

namespace editor
{

std::string GetTypeName(ShapeType type)
{
	switch (type)
	{
	case ShapeType::Rectangle:
		return "rectangle";
	case ShapeType::Ellipse:
		return "ellipse";
	case ShapeType::Triangle:
		return "triangle";
	}
	return {};
}

EditorViewModel::EditorViewModel(Picture& picture)
	: m_picture(picture)
{
	m_shapeRemovedConnection = m_picture.SubscribeToShapeRemoved([this](const Shape& shape) {
		if (std::erase(m_state.selection, shape.GetId()) > 0)
		{
			m_changedSignal();
		}
	});
	m_shapeChangedConnection = m_picture.SubscribeToShapeChanged([this](const Shape&) { m_changedSignal(); });
}

const Picture& EditorViewModel::GetPicture() const { return m_picture; }

std::vector<Rect> EditorViewModel::GetSelectedBounds() const
{
	std::vector<Rect> bounds;
	for (const auto& id : m_state.selection)
	{
		bounds.push_back(m_picture.GetShape(id).GetBounds());
	}
	return bounds;
}

bool EditorViewModel::CanDelete() const { return !m_state.selection.empty(); }

std::optional<ShapeType> EditorViewModel::GetTool() const { return m_state.tool; }

boost::signals2::connection EditorViewModel::SubscribeToChanged(const Signal::slot_type& handler)
{
	return m_changedSignal.connect(handler);
}

void EditorViewModel::SetCanvasSize(double width, double height)
{
	m_canvasWidth = width;
	m_canvasHeight = height;
}

void EditorViewModel::MouseDown(const Point& point, bool toggleSelection)
{
	FinishDrag();
	m_state.dragStart = point;

	if (m_state.tool)
	{
		BeginCreate(point);
	}
	else if (!BeginResize(point))
	{
		ClickShape(point, toggleSelection);
	}
}

void EditorViewModel::MouseMove(const Point& point)
{
	switch (m_state.mode)
	{
	case Mode::Create:
		ResizeCreatedShape(point);
		break;
	case Mode::Move:
		MoveSelection(point);
		break;
	case Mode::Resize:
		ResizeSelection(point);
		break;
	case Mode::None:
		break;
	}
}

void EditorViewModel::MouseUp(const Point& point)
{
	MouseMove(point);
	FinishDrag();
}

void EditorViewModel::Cancel()
{
	const Mode mode = std::exchange(m_state.mode, Mode::None);
	if (mode == Mode::None)
	{
		SetTool(std::nullopt);
	}
	else if (mode == Mode::Create)
	{
		m_picture.DeleteShape(m_state.createdId);
	}
	else
	{
		for (const auto& [id, bounds] : m_state.boundsBeforeDrag)
		{
			m_picture.GetShape(id).SetBounds(bounds);
		}
	}
}

void EditorViewModel::SelectTool(ShapeType tool)
{
	FinishDrag();
	SetTool(m_state.tool == tool ? std::nullopt : std::optional(tool));
}

void EditorViewModel::DeleteSelected()
{
	FinishDrag();
	if (m_state.selection.empty())
	{
		return;
	}

	auto group = std::make_unique<GroupCommand>();
	for (const auto& id : m_state.selection)
	{
		group->Add(std::make_unique<DeleteShapeCommand>(m_picture, id));
	}
	Execute(std::move(group));
}

void EditorViewModel::Undo()
{
	FinishDrag();
	m_state.selection.clear();
	m_history.Undo();
	m_changedSignal();
}

void EditorViewModel::Redo()
{
	FinishDrag();
	m_state.selection.clear();
	m_history.Redo();
	m_changedSignal();
}

std::optional<std::filesystem::path> EditorViewModel::GetFilePath() const { return m_filePath; }

void EditorViewModel::NewDocument()
{
	ReplaceDocument({});
	m_filePath.reset();
}

void EditorViewModel::OpenDocument(const std::filesystem::path& path)
{
	auto loadedShapes = DocumentIO::Load(path);
	ReplaceDocument(std::move(loadedShapes));
	m_filePath = path;
}

void EditorViewModel::SaveDocument(const std::filesystem::path& path)
{
	FinishDrag();
	DocumentIO::Save(m_picture, path);
	m_filePath = path;
}

void EditorViewModel::ReplaceDocument(std::vector<std::unique_ptr<Shape>> newShapes)
{
	FinishDrag();
	m_state = EditorState{};
	while (m_picture.GetShapeCount() > 0)
	{
		m_picture.DeleteShape(m_picture.GetShapeAt(0).GetId());
	}
	for (auto& shape : newShapes)
	{
		m_picture.AddShape(std::move(shape));
	}
	m_state.nextShapeId = m_picture.GetShapeCount() + 1;
	m_history.Clear();
	m_changedSignal();
}

void EditorViewModel::BeginCreate(const Point& point)
{
	m_state.dragStart = ClampToCanvas(point);
	m_state.createdId = GenerateShapeId();
	const Rect bounds = ClampToCanvas(MakeFrame(m_state.dragStart, m_state.dragStart));
	m_picture.AddShape(
		CreateShape(GetTypeName(*m_state.tool), m_state.createdId, Color{ 111, 222, 33 }, bounds));
	SetSelection({ m_state.createdId });
	m_state.mode = Mode::Create;
}

bool EditorViewModel::BeginResize(const Point& point)
{
	for (const auto& id : m_state.selection)
	{
		const Rect bounds = m_picture.GetShape(id).GetBounds();
		if (const auto corner = HitMarker(bounds, point))
		{
			m_state.resizeCorner = *corner;
			m_state.grabbedCorner = GetCorner(bounds, *corner);
			SaveBoundsBeforeDrag();
			m_state.mode = Mode::Resize;
			return true;
		}
	}
	return false;
}

void EditorViewModel::ClickShape(const Point& point, bool toggleSelection)
{
	const Shape* shape = m_picture.FindShapeAtPoint(point);
	if (!shape)
	{
		if (!toggleSelection)
		{
			SetSelection({});
		}
		return;
	}

	const std::string id = shape->GetId();
	auto selection = m_state.selection;
	const bool isSelected = std::ranges::find(selection, id) != selection.end();
	if (toggleSelection && isSelected)
	{
		std::erase(selection, id);
		SetSelection(std::move(selection));
		return;
	}

	if (toggleSelection)
	{
		selection.push_back(id);
	}
	else if (!isSelected)
	{
		selection = { id };
	}
	SetSelection(std::move(selection));

	SaveBoundsBeforeDrag();
	m_state.mode = Mode::Move;
}

void EditorViewModel::ResizeCreatedShape(const Point& point)
{
	const Rect bounds = ClampToCanvas(MakeFrame(m_state.dragStart, ClampToCanvas(point)));
	m_picture.GetShape(m_state.createdId).SetBounds(bounds);
}

void EditorViewModel::MoveSelection(const Point& point)
{
	double dx = point.x - m_state.dragStart.x;
	double dy = point.y - m_state.dragStart.y;
	for (const auto& [id, bounds] : m_state.boundsBeforeDrag)
	{
		dx = std::max(-bounds.left, std::min(dx, m_canvasWidth - bounds.Right()));
		dy = std::max(-bounds.top, std::min(dy, m_canvasHeight - bounds.Bottom()));
	}

	for (const auto& [id, bounds] : m_state.boundsBeforeDrag)
	{
		m_picture.GetShape(id).SetBounds({ bounds.left + dx, bounds.top + dy, bounds.width, bounds.height });
	}
}

void EditorViewModel::ResizeSelection(const Point& point)
{
	const double dx = point.x - m_state.grabbedCorner.x;
	const double dy = point.y - m_state.grabbedCorner.y;
	for (const auto& [id, bounds] : m_state.boundsBeforeDrag)
	{
		const Point corner = GetCorner(bounds, m_state.resizeCorner);
		const Point target = ClampToCanvas(Point{ corner.x + dx, corner.y + dy });
		m_picture.GetShape(id).SetBounds(ClampToCanvas(ResizeFrame(bounds, m_state.resizeCorner, target)));
	}
}

void EditorViewModel::FinishDrag()
{
	const Mode mode = std::exchange(m_state.mode, Mode::None);
	if (mode == Mode::Create)
	{
		Execute(std::make_unique<AddShapeCommand>(m_picture, m_picture.DeleteShape(m_state.createdId)));
		SetTool(std::nullopt);
		SetSelection({ m_state.createdId });
	}
	else if (mode == Mode::Move || mode == Mode::Resize)
	{
		CommitBoundsChange();
	}
}

void EditorViewModel::SaveBoundsBeforeDrag()
{
	m_state.boundsBeforeDrag.clear();
	for (const auto& id : m_state.selection)
	{
		m_state.boundsBeforeDrag[id] = m_picture.GetShape(id).GetBounds();
	}
}

void EditorViewModel::CommitBoundsChange()
{
	auto group = std::make_unique<GroupCommand>();
	for (const auto& [id, oldBounds] : m_state.boundsBeforeDrag)
	{
		const Rect newBounds = m_picture.GetShape(id).GetBounds();
		if (newBounds != oldBounds)
		{
			group->Add(std::make_unique<ChangeBoundsCommand>(m_picture, id, oldBounds, newBounds));
		}
	}

	if (!group->IsEmpty())
	{
		Execute(std::move(group));
	}
}

void EditorViewModel::Execute(std::unique_ptr<ICommand> command)
{
	m_history.Execute(std::move(command));
	m_changedSignal();
}

void EditorViewModel::SetSelection(std::vector<std::string> ids)
{
	m_state.selection = std::move(ids);
	m_changedSignal();
}

void EditorViewModel::SetTool(std::optional<ShapeType> tool)
{
	m_state.tool = tool;
	m_changedSignal();
}

std::string EditorViewModel::GenerateShapeId() { return std::format("shape{}", m_state.nextShapeId++); }

Point EditorViewModel::ClampToCanvas(const Point& point) const
{
	return { std::clamp(point.x, 0.0, m_canvasWidth), std::clamp(point.y, 0.0, m_canvasHeight) };
}

Rect EditorViewModel::ClampToCanvas(const Rect& rect) const
{
	return { std::clamp(rect.left, 0.0, std::max(0.0, m_canvasWidth - rect.width)),
		std::clamp(rect.top, 0.0, std::max(0.0, m_canvasHeight - rect.height)), rect.width, rect.height };
}

} // namespace editor
