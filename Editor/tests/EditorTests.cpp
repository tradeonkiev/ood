#include "IO/LoadService.h"
#include "IO/SaveService.h"
#include "Model/Commands/AddShape/AddShapeCommand.h"
#include "Model/Commands/DeleteShape/DeleteShapeCommand.h"
#include "Model/Commands/History.h"
#include "Model/Commands/ICommand.h"
#include "Model/Commands/MoveShape/MoveShapeCommand.h"
#include "Model/Document.h"
#include "Model/Picture.h"
#include "Model/ShapeFactory.h"
#include "View/CanvasViewport.h"
#include "ViewModel/EditorViewModel.h"
#include "ViewModel/IWindowService.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace editor;
using testing::InSequence;
using testing::StrictMock;

namespace
{

class MockCommand : public ICommand
{
public:
	MOCK_METHOD(void, Execute, (), (override));
	MOCK_METHOD(void, Unexecute, (), (override));
	std::string GetName() const override { return "MockCommand"; }
};

class MockWindowService : public IWindowService
{
public:
	MOCK_METHOD(void, OpenWindow, (std::shared_ptr<Document> document), (override));
};

std::vector<std::string> GetIds(const Picture& picture)
{
	std::vector<std::string> ids;
	for (int i = 0; i < picture.GetShapeCount(); i++)
	{
		ids.push_back(picture.GetShapeAt(i).GetId());
	}
	return ids;
}

class HistoryTest : public testing::Test
{
protected:
	MockCommand& Execute()
	{
		auto command = std::make_unique<StrictMock<MockCommand>>();
		auto& ref = *command;
		EXPECT_CALL(ref, Execute());
		history.Execute(std::move(command));
		testing::Mock::VerifyAndClearExpectations(&ref);
		return ref;
	}

	History history;
};

class CommandUndoTest : public testing::Test
{
protected:
	void AddThree()
	{
		for (const char* id : { "a", "b", "c" })
		{
			picture.AddShape(CreateShape("rectangle", id, ShapeStyle{}, { 0, 0, 10, 10 }));
		}
	}

	Picture picture;
	History history;
};

class EditorViewModelTest : public testing::Test
{
protected:
	EditorViewModelTest()
	{
		viewModel.SetCanvasSize(800, 600);
		viewModel.SelectTool(ShapeType::Rectangle);
		viewModel.MouseDown({ 100, 100 }, false);
		viewModel.MouseUp({ 200, 150 });
	}

	void Drag(Point from, Point to)
	{
		viewModel.MouseDown(from, false);
		viewModel.MouseMove(to);
		viewModel.MouseUp(to);
	}

	Rect GetBounds() const { return document->GetPicture().GetShapeAt(0).GetBounds(); }

	void AddShape(ShapeType type, Point from)
	{
		viewModel.SelectTool(type);
		viewModel.MouseDown(from, false);
		viewModel.MouseUp({ from.x + 50, from.y + 50 });
	}

	StrictMock<MockWindowService> windows;
	std::shared_ptr<Document> document = std::make_shared<Document>();
	EditorViewModel viewModel{ document, windows };
};

} // namespace

TEST_F(HistoryTest, UndoesSeveralCommandsInReverseOrder)
{
	auto& first = Execute();
	auto& second = Execute();
	auto& third = Execute();

	InSequence sequence;
	EXPECT_CALL(third, Unexecute());
	EXPECT_CALL(second, Unexecute());
	EXPECT_CALL(first, Unexecute());

	history.Undo();
	history.Undo();
	history.Undo();
}

TEST_F(HistoryTest, RedoesSeveralCommandsInOrder)
{
	auto& first = Execute();
	auto& second = Execute();
	auto& third = Execute();
	EXPECT_CALL(first, Unexecute());
	EXPECT_CALL(second, Unexecute());
	EXPECT_CALL(third, Unexecute());
	history.Undo();
	history.Undo();
	history.Undo();

	InSequence sequence;
	EXPECT_CALL(first, Execute());
	EXPECT_CALL(second, Execute());
	EXPECT_CALL(third, Execute());

	history.Redo();
	history.Redo();
	history.Redo();
}

TEST_F(HistoryTest, NewCommandAfterUndoDiscardsRedo)
{
	Execute();
	auto& undone = Execute();
	EXPECT_CALL(undone, Unexecute());
	history.Undo();

	Execute();

	EXPECT_FALSE(history.CanRedo());
	history.Redo();
}

TEST_F(HistoryTest, EmptyHistoryHasNothingToUndoOrRedo)
{
	EXPECT_FALSE(history.CanUndo());
	EXPECT_FALSE(history.CanRedo());

	history.Undo();
	history.Redo();
}

TEST_F(HistoryTest, UndoAtBeginningOfHistoryDoesNothing)
{
	auto& command = Execute();
	EXPECT_CALL(command, Unexecute()).Times(1);

	history.Undo();
	history.Undo();
	history.Undo();

	EXPECT_FALSE(history.CanUndo());
	EXPECT_TRUE(history.CanRedo());
}

TEST_F(HistoryTest, RedoAtEndOfHistoryDoesNothing)
{
	auto& command = Execute();
	EXPECT_CALL(command, Unexecute());
	history.Undo();
	EXPECT_CALL(command, Execute()).Times(1);

	history.Redo();
	history.Redo();
	history.Redo();

	EXPECT_TRUE(history.CanUndo());
	EXPECT_FALSE(history.CanRedo());
}

TEST_F(CommandUndoTest, UndoAddRemovesShapeAndRedoReturnsIt)
{
	history.Execute(
		std::make_unique<AddShapeCommand>(picture, CreateShape("rectangle", "a", ShapeStyle{}, { 5, 6, 70, 80 })));

	history.Undo();
	EXPECT_EQ(picture.GetShapeCount(), 0);

	history.Redo();
	ASSERT_EQ(picture.GetShapeCount(), 1);
	EXPECT_EQ(picture.GetShape("a").GetBounds(), (Rect{ 5, 6, 70, 80 }));
}

TEST_F(CommandUndoTest, UndoDeleteRestoresShape)
{
	picture.AddShape(CreateShape("ellipse", "1", ShapeStyle{ { 10, 20, 30 }, { 40, 50, 60 } }, { 1, 2, 30, 40 }));

	history.Execute(std::make_unique<DeleteShapeCommand>(picture, "1"));
	EXPECT_EQ(picture.GetShapeCount(), 0);

	history.Undo();
	ASSERT_EQ(picture.GetShapeCount(), 1);
	const Shape& shape = picture.GetShape("1");
	EXPECT_EQ(shape.GetType(), "ellipse");
	EXPECT_EQ(shape.GetBounds(), (Rect{ 1, 2, 30, 40 }));
	EXPECT_EQ(shape.GetStyle(), (ShapeStyle{ { 10, 20, 30 }, { 40, 50, 60 } }));
}

TEST_F(CommandUndoTest, UndoDeleteKeepsShapeOrder)
{
	AddThree();

	for (const std::string id : { "a", "b", "c" })
	{
		auto allIds = std::vector<std::string>{ "a", "b", "c" };
		history.Execute(std::make_unique<DeleteShapeCommand>(picture, id));
		std::erase_if(allIds, [id](std::string itemId) { return itemId == id; });
		EXPECT_EQ(GetIds(picture), allIds);
		history.Undo();
		EXPECT_EQ(GetIds(picture), (std::vector<std::string>{ "a", "b", "c" }));
	}
}

TEST_F(CommandUndoTest, MoveShapeChangesOrderAndUndoRestoresIt)
{
	AddThree();

	history.Execute(std::make_unique<MoveShapeCommand>(picture, "a", 2));
	EXPECT_EQ(GetIds(picture), (std::vector<std::string>{ "b", "c", "a" }));

	history.Undo();
	EXPECT_EQ(GetIds(picture), (std::vector<std::string>{ "a", "b", "c" }));

	history.Redo();
	EXPECT_EQ(GetIds(picture), (std::vector<std::string>{ "b", "c", "a" }));
}

TEST_F(CommandUndoTest, HitTestUsesNewOrder)
{
	AddThree();
	EXPECT_EQ(picture.FindShapeAtPoint({ 5, 5 }, 0)->GetId(), "c");

	picture.MoveShape("a", 2);
	EXPECT_EQ(picture.FindShapeAtPoint({ 5, 5 }, 0)->GetId(), "a");
}

TEST_F(EditorViewModelTest, PictureChangesNotifyViewModelSubscribers)
{
	int notifications = 0;
	const boost::signals2::scoped_connection connection
		= viewModel.SubscribeToChanged([&notifications] { notifications++; });

	document->GetPicture().AddShape(CreateShape("ellipse", "x", ShapeStyle{}, { 0, 0, 10, 10 }));
	EXPECT_EQ(notifications, 1);

	document->GetPicture().MoveShape("x", 0);
	EXPECT_EQ(notifications, 2);

	document->GetPicture().DeleteShape("x");
	EXPECT_EQ(notifications, 3);
}

TEST_F(EditorViewModelTest, SelectShapeFromListSelectsItOnCanvas)
{
	const std::string id = document->GetPicture().GetShapeAt(0).GetId();
	viewModel.SelectShape(id, false);
	EXPECT_TRUE(viewModel.IsSelected(id));
	EXPECT_EQ(viewModel.GetSelectedBounds(), (std::vector<Rect>{ GetBounds() }));

	viewModel.SelectShape(id, true);
	EXPECT_FALSE(viewModel.IsSelected(id));
	EXPECT_TRUE(viewModel.GetSelectedBounds().empty());
}

TEST_F(EditorViewModelTest, MoveSelectedLayersIsUndoable)
{
	AddShape(ShapeType::Ellipse, { 300, 300 });
	AddShape(ShapeType::Triangle, { 500, 300 });
	const auto before = GetIds(document->GetPicture());

	viewModel.SelectShape(before[0], false);
	viewModel.SelectShape(before[1], true);
	viewModel.MoveSelectedLayers(1);
	EXPECT_EQ(GetIds(document->GetPicture()), (std::vector<std::string>{ before[2], before[0], before[1] }));

	viewModel.MoveSelectedLayers(1);
	EXPECT_EQ(GetIds(document->GetPicture()), (std::vector<std::string>{ before[2], before[0], before[1] }));

	viewModel.MoveSelectedLayers(-1);
	EXPECT_EQ(GetIds(document->GetPicture()), before);

	viewModel.Undo();
	EXPECT_EQ(GetIds(document->GetPicture()), (std::vector<std::string>{ before[2], before[0], before[1] }));
	EXPECT_TRUE(viewModel.IsSelected(before[0]));
	EXPECT_TRUE(viewModel.IsSelected(before[1]));
}

TEST_F(EditorViewModelTest, UndoSelectsAffectedShapes)
{
	const std::string first = document->GetPicture().GetShapeAt(0).GetId();
	AddShape(ShapeType::Ellipse, { 300, 300 });
	viewModel.SelectShape(first, false);
	viewModel.SetFillColor({ 1, 2, 3 });
	viewModel.SelectShape(document->GetPicture().GetShapeAt(1).GetId(), false);

	viewModel.Undo();
	EXPECT_TRUE(viewModel.IsSelected(first));
	EXPECT_EQ(viewModel.GetSelectedBounds().size(), 1u);
}

TEST_F(EditorViewModelTest, CurrentTimeIsNotInHistory)
{
	viewModel.Undo();
	ASSERT_FALSE(document->GetHistory().CanUndo());

	viewModel.SetCurrentTime(4.5);
	EXPECT_DOUBLE_EQ(viewModel.GetCurrentTime(), 4.5);
	EXPECT_FALSE(document->GetHistory().CanUndo());
}

TEST_F(EditorViewModelTest, LifetimeChangeIsUndoableAndHidesShape)
{
	const std::string id = document->GetPicture().GetShapeAt(0).GetId();
	viewModel.BeginLifetimeResize(id, LifetimeEdge::Start);
	viewModel.ResizeLifetime(3);
	viewModel.EndLifetimeResize();
	EXPECT_EQ(document->GetPicture().GetShape(id).GetLifetime(), (LifeStyle{ 3, MaxTime }));

	viewModel.SetCurrentTime(1);
	viewModel.MouseDown({ 150, 120 }, false);
	viewModel.MouseUp({ 150, 120 });
	EXPECT_FALSE(viewModel.IsSelected(id));

	viewModel.Undo();
	EXPECT_EQ(document->GetPicture().GetShape(id).GetLifetime(), LifeStyle{});
}

TEST_F(EditorViewModelTest, ResizeLifetimeByEdgeIsPreviewedThenUndoable)
{
	const std::string id = document->GetPicture().GetShapeAt(0).GetId();
	viewModel.BeginLifetimeResize(id, LifetimeEdge::End);
	viewModel.ResizeLifetime(4);
	EXPECT_EQ(viewModel.GetShapeLifetime(id), (LifeStyle{ 0, 4 }));
	EXPECT_EQ(document->GetPicture().GetShape(id).GetLifetime(), LifeStyle{});

	viewModel.EndLifetimeResize();
	EXPECT_EQ(document->GetPicture().GetShape(id).GetLifetime(), (LifeStyle{ 0, 4 }));

	viewModel.Undo();
	EXPECT_EQ(document->GetPicture().GetShape(id).GetLifetime(), LifeStyle{});
}

TEST_F(EditorViewModelTest, WindowsOfOneDocumentHaveOwnSelectionTimeAndPreview)
{
	EditorViewModel other{ document, windows };
	other.SetCanvasSize(800, 600);
	const std::string id = document->GetPicture().GetShapeAt(0).GetId();
	other.SelectShape(id, false);
	other.SetCurrentTime(2);

	viewModel.MouseDown({ 150, 120 }, false);
	viewModel.MouseMove({ 250, 220 });
	EXPECT_EQ(GetBounds(), (Rect{ 100, 100, 100, 50 }));
	EXPECT_EQ(other.GetSelectedBounds(), (std::vector<Rect>{ { 100, 100, 100, 50 } }));
	EXPECT_DOUBLE_EQ(viewModel.GetCurrentTime(), 0);

	viewModel.MouseUp({ 250, 220 });
	EXPECT_EQ(other.GetSelectedBounds(), (std::vector<Rect>{ { 200, 200, 100, 50 } }));

	viewModel.DeleteSelected();
	EXPECT_FALSE(other.IsSelected(id));
	EXPECT_TRUE(other.GetSelectedBounds().empty());

	other.Undo();
	EXPECT_TRUE(other.IsSelected(id));
	EXPECT_FALSE(viewModel.IsSelected(id));
}

TEST_F(EditorViewModelTest, NewShapeGetsLastChosenColor)
{
	viewModel.SelectShape(document->GetPicture().GetShapeAt(0).GetId(), false);
	viewModel.SetFillColor({ 10, 20, 30 });
	viewModel.SetStrokeColor({ 40, 50, 60 });

	AddShape(ShapeType::Ellipse, { 300, 300 });
	EXPECT_EQ(document->GetPicture().GetShapeAt(1).GetStyle(), (ShapeStyle{ { 10, 20, 30 }, { 40, 50, 60 } }));
}

TEST_F(EditorViewModelTest, UndoMoveRestoresPosition)
{
	Drag({ 150, 120 }, { 250, 220 });
	EXPECT_EQ(GetBounds(), (Rect{ 200, 200, 100, 50 }));

	viewModel.Undo();
	EXPECT_EQ(GetBounds(), (Rect{ 100, 100, 100, 50 }));

	viewModel.Redo();
	EXPECT_EQ(GetBounds(), (Rect{ 200, 200, 100, 50 }));
}

TEST_F(EditorViewModelTest, UndoResizeRestoresBounds)
{
	Drag({ 200, 150 }, { 300, 300 });
	EXPECT_EQ(GetBounds(), (Rect{ 100, 100, 200, 200 }));

	viewModel.Undo();
	EXPECT_EQ(GetBounds(), (Rect{ 100, 100, 100, 50 }));

	viewModel.Redo();
	EXPECT_EQ(GetBounds(), (Rect{ 100, 100, 200, 200 }));
}

TEST(DocumentIOTest, SaveAndLoadKeepTypesBoundsColorsAndOrder)
{
	Picture picture;
	picture.AddShape(CreateShape("rectangle", "r1", ShapeStyle{ { 255, 0, 16 }, { 0, 0, 0 } }, { 10, 20, 100, 50 }));
	picture.AddShape(CreateShape("ellipse", "e1", ShapeStyle{ { 0, 128, 0 }, { 255, 255, 255 } }, { 200.5, 200, 80, 40 }));
	picture.AddShape(CreateShape("triangle", "t1", ShapeStyle{ { 1, 2, 3 }, { 4, 5, 6 } }, { 300, 100, 60, 90 }, { 1.5, 7 }));

	std::stringstream stream;
	SaveService::Save(picture, stream);
	const auto loaded = LoadService::Load(stream);

	ASSERT_EQ(static_cast<int>(loaded.size()), picture.GetShapeCount());
	for (int i = 0; i < picture.GetShapeCount(); i++)
	{
		EXPECT_EQ(loaded[i]->GetType(), picture.GetShapeAt(i).GetType());
		EXPECT_EQ(loaded[i]->GetBounds(), picture.GetShapeAt(i).GetBounds());
		EXPECT_EQ(loaded[i]->GetStyle(), picture.GetShapeAt(i).GetStyle());
		EXPECT_EQ(loaded[i]->GetLifetime(), picture.GetShapeAt(i).GetLifetime());
	}
}

TEST(CanvasViewportTest, DefaultViewportMapsPointsAsIs)
{
	const CanvasViewport viewport;

	EXPECT_EQ(viewport.ToPicture({ 10, 20 }), (Point{ 10, 20 }));
	EXPECT_EQ(viewport.ToScreen({ 10, 20 }), (Point{ 10, 20 }));
}

TEST(CanvasViewportTest, ZoomKeepsPointUnderAnchor)
{
	CanvasViewport viewport;
	const Point anchor{ 100, 50 };
	const Point before = viewport.ToPicture(anchor);

	viewport.ZoomAt(anchor, 2);

	EXPECT_DOUBLE_EQ(viewport.GetZoom(), 2);
	EXPECT_EQ(viewport.ToPicture(anchor), before);
	EXPECT_EQ(viewport.ToScreen({ 110, 50 }), (Point{ 120, 50 }));
}

TEST(CanvasViewportTest, ZoomIsClamped)
{
	CanvasViewport viewport;

	viewport.ZoomAt({ 0, 0 }, 1000);
	EXPECT_DOUBLE_EQ(viewport.GetZoom(), MaxZoom);

	viewport.ZoomAt({ 0, 0 }, 0.00001);
	EXPECT_DOUBLE_EQ(viewport.GetZoom(), MinZoom);
}

TEST(CanvasViewportTest, MoveViewIsMeasuredInScreenPixels)
{
	CanvasViewport viewport;
	viewport.ZoomAt({ 0, 0 }, 2);

	viewport.MoveView(40, -20);

	EXPECT_EQ(viewport.GetOffset(), (Point{ 20, -10 }));
	EXPECT_EQ(viewport.ToScreen({ 20, -10 }), (Point{ 0, 0 }));
}

// TEST()