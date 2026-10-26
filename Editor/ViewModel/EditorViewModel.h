#pragma once

#include "Model/Commands/History.h"
#include "Model/Picture.h"
#include "ViewModel/SelectionFrame.h"
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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
	Create,
};

struct EditorState
{
	std::vector<std::string> selection;
	std::optional<ShapeType> tool;
	Mode mode = Mode::None;
	Point dragStart;
	std::map<std::string, Rect> boundsBeforeDrag;
	Corner resizeCorner = Corner::BottomRight;
	Point grabbedCorner;
	std::string createdId;
	int nextShapeId = 1;
};

class EditorViewModel
{
public:
	using Signal = boost::signals2::signal<void()>;

	explicit EditorViewModel(Picture& picture);

	const Picture& GetPicture() const;
	std::vector<Rect> GetSelectedBounds() const;
	bool CanDelete() const;
	std::optional<ShapeType> GetTool() const;

	boost::signals2::connection SubscribeToChanged(const Signal::slot_type& handler);

	void SetCanvasSize(double width, double height);
	void MouseDown(const Point& point, bool toggleSelection);
	void MouseMove(const Point& point);
	void MouseUp(const Point& point);
	void Cancel();

	void SelectTool(ShapeType tool);
	void DeleteSelected();
	void Undo();
	void Redo();

	std::optional<std::filesystem::path> GetFilePath() const;
	void NewDocument();
	void OpenDocument(const std::filesystem::path& path);
	void SaveDocument(const std::filesystem::path& path);

private:
	void ReplaceDocument(std::vector<std::unique_ptr<Shape>> newShapes);
	void BeginCreate(const Point& point);
	bool BeginResize(const Point& point);
	void ClickShape(const Point& point, bool toggleSelection);

	void ResizeCreatedShape(const Point& point);
	void MoveSelection(const Point& point);
	void ResizeSelection(const Point& point);

	void FinishDrag();
	void SaveBoundsBeforeDrag();
	void CommitBoundsChange();

	void Execute(std::unique_ptr<ICommand> command);
	void SetSelection(std::vector<std::string> ids);
	void SetTool(std::optional<ShapeType> tool);

	std::string GenerateShapeId();
	Point ClampToCanvas(const Point& point) const;
	Rect ClampToCanvas(const Rect& rect) const;

	Picture& m_picture;
	History m_history;

	EditorState m_state;
	double m_canvasWidth = 0;
	double m_canvasHeight = 0;
	std::optional<std::filesystem::path> m_filePath;

	Signal m_changedSignal;
	boost::signals2::scoped_connection m_shapeRemovedConnection;
	boost::signals2::scoped_connection m_shapeChangedConnection;
};

} // namespace editor
