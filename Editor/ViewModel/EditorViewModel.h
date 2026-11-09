#pragma once

#include "Canvas/ICanvas.h"
#include "Constants/EditorConstants.h"
#include "Model/Document.h"
#include "ViewModel/IWindowService.h"
#include "ViewModel/SelectionFrame.h"
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <filesystem>
#include <functional>
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
	ResizeLifetime,
};

enum class LifetimeEdge
{
	Start,
	End,
};

struct EditorState
{
	std::vector<std::string> selection;
	std::optional<ShapeType> tool;
	Mode mode = Mode::None;
	Point dragStart;
	std::map<std::string, Rect> boundsBeforeDrag;
	std::map<std::string, Rect> previewBounds;
	Corner resizeCorner = Corner::BottomRight;
	Point grabbedCorner;
	std::string createdId;
	Rect createdBounds;
	std::string lifetimeId;
	LifetimeEdge lifetimeEdge = LifetimeEdge::End;
	LifeStyle previewLifetime;
	double currentTime = 0;
	ShapeStyle newShapeStyle{ DefaultFillColor, DefaultStrokeColor };
};

class EditorViewModel
{
public:
	using Signal = boost::signals2::signal<void()>;

	EditorViewModel(std::shared_ptr<Document> document, IWindowService& windows);

	const Picture& GetPicture() const;
	void DrawPicture(ICanvas& canvas) const;
	Rect GetShapeBounds(const std::string& id) const;
	LifeStyle GetShapeLifetime(const std::string& id) const;
	std::vector<Rect> GetSelectedBounds() const;
	bool CanDelete() const;
	bool IsSelected(const std::string& id) const;
	ShapeStyle GetCurrentStyle() const;
	std::optional<ShapeType> GetTool() const;
	std::string GetTitle() const;
	double GetCurrentTime() const;

	boost::signals2::connection SubscribeToChanged(const Signal::slot_type& handler);

	void SetCanvasSize(double width, double height);
	void MouseDown(const Point& point, bool toggleSelection);
	void MouseMove(const Point& point);
	void MouseUp(const Point& point);
	void Cancel();

	void SelectTool(ShapeType tool);
	void SelectShape(const std::string& id, bool toggleSelection);
	void MoveSelectedLayers(int delta);
	void GroupSelected();
	void UngroupSelected();
	void SetCurrentTime(double time);
	void BeginLifetimeResize(const std::string& id, LifetimeEdge edge);
	void ResizeLifetime(double time);
	void EndLifetimeResize();
	void DeleteSelected();
	void SetFillColor(Color color);
	void SetStrokeColor(Color color);
	void Undo();
	void Redo();

	std::optional<std::filesystem::path> GetFilePath() const;
	void SaveDocument(const std::filesystem::path& path);
	void NewDocument();
	void OpenDocument(const std::filesystem::path& path);
	void OpenNewWindow();

private:
	void StartCreate(const Point& point);
	bool BeginResize(const Point& point);
	void ClickShape(const Point& point, bool toggleSelection);

	void ResizeCreatedShape(const Point& point);
	void MoveSelection(const Point& point);
	void ResizeSelection(const Point& point);

	void ChangeSelectedStyle(const std::function<void(ShapeStyle& style)>& change);

	void FinishDrag();
	void SaveBoundsBeforeDrag();
	void CommitBoundsChange();
	void ForgetShape(const std::string& id);

	void Execute(std::unique_ptr<ICommand> command);
	void SelectAffectedShapes(const ICommand* command);
	void SetSelection(std::vector<std::string> ids);
	void SetTool(std::optional<ShapeType> tool);

	Point ClampToCanvas(const Point& point) const;
	Rect ClampToCanvas(const Rect& rect) const;

	std::shared_ptr<Document> m_document;
	IWindowService& m_windows;
	Picture& m_picture;
	History& m_history;

	EditorState m_state;
	double m_canvasWidth = 0;
	double m_canvasHeight = 0;

	Signal m_changedSignal;
	boost::signals2::scoped_connection m_shapeAddedConnection;
	boost::signals2::scoped_connection m_shapeRemovedConnection;
	boost::signals2::scoped_connection m_shapeChangedConnection;
	boost::signals2::scoped_connection m_shapeMovedConnection;
};

} // namespace editor
