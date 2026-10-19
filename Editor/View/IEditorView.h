#pragma once

#include "Geometry/Geometry.h"
#include "Picture/Picture.h"

#include <optional>

namespace editor
{

enum class ShapeType
{
	Rectangle,
	Ellipse,
	Triangle,
};

class IEditorViewListener
{
public:
	virtual ~IEditorViewListener() = default;
	virtual void OnPaint() = 0;
	virtual void OnMouseDown(const shapes::Point& point) = 0;
	virtual void OnMouseMove(const shapes::Point& point) = 0;
	virtual void OnMouseUp(const shapes::Point& point) = 0;
	virtual void OnAddShape(ShapeType type) = 0;
	virtual void OnDeleteSelected() = 0;
	virtual void OnUndo() = 0;
	virtual void OnRedo() = 0;
};

class IEditorView
{
public:
	virtual ~IEditorView() = default;

	virtual void SetListener(IEditorViewListener* listener) = 0;

	virtual void Render(const shapes::Picture& picture, const std::optional<shapes::Rect>& selectionFrame) = 0;
};

} // namespace editor
