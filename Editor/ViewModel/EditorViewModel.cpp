#include "EditorViewModel.h"

#include "IO/DocumentIO.h"
#include "Model/Commands/AddShape/AddShapeCommand.h"
#include "Model/Commands/ChangeBounds/ChangeBoundsCommand.h"
#include "Model/Commands/ChangeLifetime/ChangeLifetimeCommand.h"
#include "Model/Commands/ChangeStyle/ChangeStyleCommand.h"
#include "Model/Commands/DeleteShape/DeleteShapeCommand.h"
#include "Model/Commands/Group/GroupCommand.h"
#include "Model/Commands/GroupShapes/GroupShapesCommand.h"
#include "Model/Commands/MoveShape/MoveShapeCommand.h"
#include "Model/Commands/UngroupShapes/UngroupShapesCommand.h"
#include "Model/ShapeFactory.h"
#include "Model/Shapes/GroupShape.h"

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

void DrawPreview(ICanvas& canvas, const Shape& shape, const Rect& bounds)
{
	const GroupShape* group = shape.GetGroup();
	if (!group)
	{
		CreateShape(shape.GetType(), shape.GetId(), shape.GetStyle(), bounds)->Draw(canvas);
		return;
	}

	const Rect old = shape.GetBounds();
	for (int i = 0; i < group->GetShapeCount(); i++)
	{
		const Shape& child = group->GetShapeAt(i);
		DrawPreview(canvas, child, MapRect(child.GetBounds(), old, bounds));
	}
}

void CollectLeafIds(const Shape& shape, std::vector<std::string>& ids)
{
	const GroupShape* group = shape.GetGroup();
	if (!group)
	{
		ids.push_back(shape.GetId());
		return;
	}
	for (int i = 0; i < group->GetShapeCount(); i++)
	{
		CollectLeafIds(group->GetShapeAt(i), ids);
	}
}

EditorViewModel::EditorViewModel(std::shared_ptr<Document> document, IWindowService& windows)
	: m_document(std::move(document))
	, m_windows(windows)
	, m_picture(m_document->GetPicture())
	, m_history(m_document->GetHistory())
{
	m_shapeAddedConnection = m_picture.SubscribeToShapeAdded([this](const Shape&) { m_changedSignal(); });

	m_shapeRemovedConnection = m_picture.SubscribeToShapeRemoved([this](const Shape& shape) {
		ForgetShape(shape.GetId());
		m_changedSignal();
	});

	m_shapeChangedConnection = m_picture.SubscribeToShapeChanged([this](const Shape&) { m_changedSignal(); });

	m_shapeMovedConnection = m_picture.SubscribeToShapeMoved([this](const Shape&) { m_changedSignal(); });
}

const Picture& EditorViewModel::GetPicture() const { return m_picture; }

void EditorViewModel::DrawPicture(ICanvas& canvas) const
{
	canvas.Clear();
	for (int i = 0; i < m_picture.GetShapeCount(); i++)
	{
		const Shape& shape = m_picture.GetShapeAt(i);
		if (!GetShapeLifetime(shape.GetId()).Contains(m_state.currentTime))
		{
			continue;
		}

		const auto preview = m_state.previewBounds.find(shape.GetId());
		if (preview == m_state.previewBounds.end())
		{
			shape.Draw(canvas);
		}
		else
		{
			DrawPreview(canvas, shape, preview->second);
		}
	}

	if (m_state.mode == Mode::Create)
	{
		CreateShape(GetTypeName(*m_state.tool), m_state.createdId, m_state.newShapeStyle, m_state.createdBounds)->Draw(canvas);
	}
}

Rect EditorViewModel::GetShapeBounds(const std::string& id) const
{
	const auto preview = m_state.previewBounds.find(id);
	return preview != m_state.previewBounds.end() ? preview->second : m_picture.GetShape(id).GetBounds();
}

LifeStyle EditorViewModel::GetShapeLifetime(const std::string& id) const
{
	if (m_state.mode == Mode::ResizeLifetime && m_state.lifetimeId == id)
	{
		return m_state.previewLifetime;
	}
	return m_picture.GetShape(id).GetLifetime();
}

std::vector<Rect> EditorViewModel::GetSelectedBounds() const
{
	std::vector<Rect> bounds;
	for (const auto& id : m_state.selection)
	{
		bounds.push_back(GetShapeBounds(id));
	}
	if (m_state.mode == Mode::Create)
	{
		bounds.push_back(m_state.createdBounds);
	}

	return bounds;
}

bool EditorViewModel::CanDelete() const { return !m_state.selection.empty(); }

bool EditorViewModel::IsSelected(const std::string& id) const
{
	return std::ranges::find(m_state.selection, id) != m_state.selection.end();
}

ShapeStyle EditorViewModel::GetCurrentStyle() const
{
	if (m_state.selection.empty())
	{
		return m_state.newShapeStyle;
	}

	return m_picture.GetShape(m_state.selection.front()).GetStyle();
}

std::optional<ShapeType> EditorViewModel::GetTool() const { return m_state.tool; }

std::string EditorViewModel::GetTitle() const { return m_document->GetTitle(); }

double EditorViewModel::GetCurrentTime() const { return m_state.currentTime; }

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
		StartCreate(point);
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
	case Mode::ResizeLifetime:
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
		return;
	}

	m_state.boundsBeforeDrag.clear();
	m_state.previewBounds.clear();
	m_changedSignal();
}

void EditorViewModel::SelectTool(ShapeType tool)
{
	FinishDrag();
	SetTool(m_state.tool == tool ? std::nullopt : std::optional(tool));
}

void EditorViewModel::SelectShape(const std::string& id, bool toggleSelection)
{
	FinishDrag();
	if (!toggleSelection)
	{
		SetSelection({ id });
		return;
	}

	std::vector<std::string> selection = m_state.selection;
	if (std::erase(selection, id) == 0)
	{
		selection.push_back(id);
	}
	SetSelection(std::move(selection));
}

void EditorViewModel::MoveSelectedLayers(int delta)
{
	FinishDrag();
	std::vector<std::string> current;
	for (int i = 0; i < m_picture.GetShapeCount(); i++)
	{
		current.push_back(m_picture.GetShapeAt(i).GetId());
	}

	const int count = static_cast<int>(current.size());
	const bool up = delta > 0;
	int limit = up ? count : -1;
	auto group = std::make_unique<GroupCommand>();
	for (int step = 0; step < count; step++)
	{
		const int index = up ? count - 1 - step : step;
		const std::string id = current[index];
		if (!IsSelected(id))
		{
			continue;
		}

		const int target = up ? std::min(index + 1, limit - 1) : std::max(index - 1, limit + 1);
		if (target != index)
		{
			current.erase(current.begin() + index);
			current.insert(current.begin() + target, id);
			group->Add(std::make_unique<MoveShapeCommand>(m_picture, id, target));
		}
		limit = target;
	}

	if (!group->IsEmpty())
	{
		Execute(std::move(group));
	}
}

void EditorViewModel::GroupSelected()
{
	FinishDrag();
	if (m_state.selection.size() < 2)
	{
		return;
	}

	std::vector<std::string> ids;
	for (int i = 0; i < m_picture.GetShapeCount(); i++)
	{
		const std::string& id = m_picture.GetShapeAt(i).GetId();
		if (IsSelected(id))
		{
			ids.push_back(id);
		}
	}

	const std::string groupId = m_document->GenerateShapeId();
	Execute(std::make_unique<GroupShapesCommand>(m_picture, groupId, std::move(ids)));
	SetSelection({ groupId });
}

void EditorViewModel::UngroupSelected()
{
	FinishDrag();
	std::vector<std::string> freed;
	auto group = std::make_unique<GroupCommand>();
	for (const auto& id : m_state.selection)
	{
		const GroupShape* shape = m_picture.GetShape(id).GetGroup();
		if (!shape)
		{
			freed.push_back(id);
			continue;
		}
		for (int i = 0; i < shape->GetShapeCount(); i++)
		{
			freed.push_back(shape->GetShapeAt(i).GetId());
		}
		group->Add(std::make_unique<UngroupShapesCommand>(m_picture, id));
	}

	if (!group->IsEmpty())
	{
		Execute(std::move(group));
		SetSelection(std::move(freed));
	}
}

void EditorViewModel::SetCurrentTime(double time)
{
	time = std::clamp(time, 0.0, MaxTime);
	if (time != m_state.currentTime)
	{
		m_state.currentTime = time;
		m_changedSignal();
	}
}

void EditorViewModel::BeginLifetimeResize(const std::string& id, LifetimeEdge edge)
{
	FinishDrag();
	m_state.lifetimeId = id;
	m_state.lifetimeEdge = edge;
	m_state.previewLifetime = m_picture.GetShape(id).GetLifetime();
	m_state.mode = Mode::ResizeLifetime;
}

void EditorViewModel::ResizeLifetime(double time)
{
	if (m_state.mode != Mode::ResizeLifetime)
	{
		return;
	}

	LifeStyle& lifetime = m_state.previewLifetime;
	if (m_state.lifetimeEdge == LifetimeEdge::Start)
	{
		lifetime.start = std::clamp(time, 0.0, lifetime.end - MinDuration);
	}
	else
	{
		lifetime.end = std::clamp(time, lifetime.start + MinDuration, MaxTime);
	}
	m_changedSignal();
}

void EditorViewModel::EndLifetimeResize()
{
	if (m_state.mode == Mode::ResizeLifetime)
	{
		FinishDrag();
	}
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

void EditorViewModel::SetFillColor(Color color)
{
	ChangeSelectedStyle([color](ShapeStyle& style) { style.fillColor = color; });
}

void EditorViewModel::SetStrokeColor(Color color)
{
	ChangeSelectedStyle([color](ShapeStyle& style) { style.strokeColor = color; });
}

void EditorViewModel::Undo()
{
	FinishDrag();
	SelectAffectedShapes(m_history.Undo());
}

void EditorViewModel::Redo()
{
	FinishDrag();
	SelectAffectedShapes(m_history.Redo());
}

std::optional<std::filesystem::path> EditorViewModel::GetFilePath() const { return m_document->GetFilePath(); }

void EditorViewModel::SaveDocument(const std::filesystem::path& path)
{
	FinishDrag();
	DocumentIO::Save(m_picture, path);
	m_document->SetFilePath(path);
}

void EditorViewModel::NewDocument() { m_windows.OpenWindow(std::make_shared<Document>()); }

void EditorViewModel::OpenDocument(const std::filesystem::path& path)
{
	m_windows.OpenWindow(std::make_shared<Document>(DocumentIO::Load(path), path));
}

void EditorViewModel::OpenNewWindow() { m_windows.OpenWindow(m_document); }

void EditorViewModel::StartCreate(const Point& point)
{
	m_state.dragStart = ClampToCanvas(point);
	m_state.createdId = m_document->GenerateShapeId();
	m_state.createdBounds = ClampToCanvas(MakeFrame(m_state.dragStart, m_state.dragStart));
	m_state.mode = Mode::Create;
	SetSelection({});
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
	const Shape* shape = m_picture.FindShapeAtPoint(point, m_state.currentTime);
	if (!shape)
	{
		if (!toggleSelection)
		{
			SetSelection({});
		}
		return;
	}

	std::vector<std::string> selection = m_state.selection;
	const std::string id = shape->GetId();
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
	m_state.createdBounds = ClampToCanvas(MakeFrame(m_state.dragStart, ClampToCanvas(point)));
	m_changedSignal();
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
		m_state.previewBounds[id] = { bounds.left + dx, bounds.top + dy, bounds.width, bounds.height };
	}
	m_changedSignal();
}

void EditorViewModel::ResizeSelection(const Point& point)
{
	const double dx = point.x - m_state.grabbedCorner.x;
	const double dy = point.y - m_state.grabbedCorner.y;
	for (const auto& [id, bounds] : m_state.boundsBeforeDrag)
	{
		const Point corner = GetCorner(bounds, m_state.resizeCorner);
		const Point target = ClampToCanvas(Point{ corner.x + dx, corner.y + dy });
		m_state.previewBounds[id] = ClampToCanvas(ResizeFrame(bounds, m_state.resizeCorner, target));
	}
	m_changedSignal();
}

void EditorViewModel::ChangeSelectedStyle(const std::function<void(ShapeStyle& style)>& change)
{
	FinishDrag();
	change(m_state.newShapeStyle);
	std::vector<std::string> leafIds;
	for (const auto& id : m_state.selection)
	{
		CollectLeafIds(m_picture.GetShape(id), leafIds);
	}

	auto group = std::make_unique<GroupCommand>();
	for (const auto& id : leafIds)
	{
		const ShapeStyle oldStyle = m_picture.GetShape(id).GetStyle();
		ShapeStyle newStyle = oldStyle;
		change(newStyle);
		if (newStyle != oldStyle)
		{
			group->Add(std::make_unique<ChangeStyleCommand>(m_picture, id, oldStyle, newStyle));
		}
	}

	if (group->IsEmpty())
	{
		m_changedSignal();
		return;
	}
	Execute(std::move(group));
}

void EditorViewModel::FinishDrag()
{
	const Mode mode = std::exchange(m_state.mode, Mode::None);
	if (mode == Mode::Create)
	{
		Execute(std::make_unique<AddShapeCommand>(m_picture,
			CreateShape(GetTypeName(*m_state.tool), m_state.createdId, m_state.newShapeStyle, m_state.createdBounds)));
		SetTool(std::nullopt);
		SetSelection({ m_state.createdId });
	}
	else if (mode == Mode::Move || mode == Mode::Resize)
	{
		CommitBoundsChange();
	}
	else if (mode == Mode::ResizeLifetime)
	{
		const LifeStyle oldLifetime = m_picture.GetShape(m_state.lifetimeId).GetLifetime();
		if (m_state.previewLifetime != oldLifetime)
		{
			Execute(std::make_unique<ChangeLifetimeCommand>(
				m_picture, m_state.lifetimeId, oldLifetime, m_state.previewLifetime));
		}
		else
		{
			m_changedSignal();
		}
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
	m_state.boundsBeforeDrag.clear();
	const auto previews = std::exchange(m_state.previewBounds, {});

	auto group = std::make_unique<GroupCommand>();
	for (const auto& [id, newBounds] : previews)
	{
		const Rect oldBounds = m_picture.GetShape(id).GetBounds();
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

void EditorViewModel::ForgetShape(const std::string& id)
{
	std::erase(m_state.selection, id);
	m_state.boundsBeforeDrag.erase(id);
	m_state.previewBounds.erase(id);
	if (m_state.mode == Mode::ResizeLifetime)
	{
		m_state.mode = Mode::None;
	}
}

void EditorViewModel::Execute(std::unique_ptr<ICommand> command)
{
	m_history.Execute(std::move(command));
	m_changedSignal();
}

void EditorViewModel::SelectAffectedShapes(const ICommand* command)
{
	std::vector<std::string> ids;
	if (command)
	{
		for (const auto& id : command->GetShapeIds())
		{
			const auto rootId = m_picture.FindRootId(id);
			if (rootId && std::ranges::find(ids, *rootId) == ids.end())
			{
				ids.push_back(*rootId);
			}
		}
	}
	SetSelection(std::move(ids));
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
