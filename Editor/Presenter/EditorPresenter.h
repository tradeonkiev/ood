#pragma once

#include "Model/Commands/History.h"
#include "Picture/Picture.h"
#include "Presenter/SelectionFrame.h"
#include "View/IEditorView.h"

#include <boost/signals2/connection.hpp>

#include <optional>
#include <string>

namespace ebator
{

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

class EditorPresenter : public IEditorViewListener
{
public:
	EditorPresenter(shapes::Picture& picture, IEditorView& view);
	~EditorPresenter() override;

	EditorPresenter(const EditorPresenter&) = delete;
	EditorPresenter& operator=(const EditorPresenter&) = delete;

	void OnPaint() override;

	void OnMouseDown(const shapes::Point& point) override;
	void OnMouseMove(const shapes::Point& point) override;
	void OnMouseUp(const shapes::Point& point) override;

	void OnAddShape(ShapeType type) override;
	void OnDeleteSelected() override;
	void OnUndo() override;
	void OnRedo() override;

private:
	std::optional<shapes::Rect> GetSelectionFrame() const;
	void FinishDrag();

	std::string GenerateShapeId();

	shapes::Picture& m_picture;
	IEditorView& m_view;
	EditorState m_state;
	History m_history;
	boost::signals2::scoped_connection m_shapeRemovedConnection;
};

} // namespace ebator
