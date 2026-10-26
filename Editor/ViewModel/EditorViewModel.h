#pragma once

#include "Model/Commands/History.h"
#include "Picture/Picture.h"
#include "ViewModel/ObservableValue.h"
#include "ViewModel/SelectionFrame.h"
#include <boost/signals2/connection.hpp>
#include <optional>
#include <string>

namespace editor
{

enum class ShapeType
{
	Rectangle,
	Ellipse,
	Triangle,
};

enum class Mode
{
	None,
	Move,
	Resize,
};

struct EditorState
{
	std::optional<std::string> selectedId;
	Mode dragMode = Mode::None;
	shapes::Point lastMousePoint;
	shapes::Rect boundsBeforeDrag;
	Corner resizeCorner = Corner::BottomRight;
	int nextShapeId = 1;
};

class EditorViewModel
{
public:
	explicit EditorViewModel(shapes::Picture& picture);

	const shapes::Picture& GetPicture() const;
	const ObservableValue<std::optional<shapes::Rect>>& GetSelectionFrame() const;
	const ObservableValue<bool>& CanDelete() const;
	const ObservableValue<bool>& CanUndo() const;
	const ObservableValue<bool>& CanRedo() const;

	void MouseDown(const shapes::Point& point);
	void MouseMove(const shapes::Point& point);
	void MouseUp(const shapes::Point& point);

	void AddShape(ShapeType type);
	void DeleteSelected();
	void Undo();
	void Redo();

private:
	void Select(std::optional<std::string> id);
	void UpdateSelectionFrame();
	void ExecuteInHistory(std::unique_ptr<ICommand> command);
	void UpdateHistoryState();
	void FinishDrag();

	std::string GenerateShapeId();

	shapes::Picture& m_picture;
	EditorState m_state;
	History m_history;

	ObservableValue<std::optional<shapes::Rect>> m_selectionFrame;
	ObservableValue<bool> m_canDelete;
	ObservableValue<bool> m_canUndo;
	ObservableValue<bool> m_canRedo;

	boost::signals2::scoped_connection m_shapeRemovedConnection;
	boost::signals2::scoped_connection m_shapeMovedConnection;
	boost::signals2::scoped_connection m_shapeGeometryChangedConnection;
};

} // namespace editor
