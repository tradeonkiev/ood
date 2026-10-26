#include "IO/LoadService.h"
#include "IO/SaveService.h"
#include "Model/Commands/AddShape/AddShapeCommand.h"
#include "Model/Commands/DeleteShape/DeleteShapeCommand.h"
#include "Model/Commands/History.h"
#include "Model/Commands/ICommand.h"
#include "Model/Document.h"
#include "Model/Picture.h"
#include "Model/ShapeFactory.h"
#include "ViewModel/EditorViewModel.h"
#include "ViewModel/IWindowService.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
			picture.AddShape(CreateShape("rectangle", id, Color{}, { 0, 0, 10, 10 }));
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
		std::make_unique<AddShapeCommand>(picture, CreateShape("rectangle", "a", Color{}, { 5, 6, 70, 80 })));

	history.Undo();
	EXPECT_EQ(picture.GetShapeCount(), 0);

	history.Redo();
	ASSERT_EQ(picture.GetShapeCount(), 1);
	EXPECT_EQ(picture.GetShape("a").GetBounds(), (Rect{ 5, 6, 70, 80 }));
}

TEST_F(CommandUndoTest, UndoDeleteRestoresShape)
{
	picture.AddShape(CreateShape("ellipse", "e", Color{ 10, 20, 30 }, { 1, 2, 30, 40 }));

	history.Execute(std::make_unique<DeleteShapeCommand>(picture, "e"));
	EXPECT_EQ(picture.GetShapeCount(), 0);

	history.Undo();
	ASSERT_EQ(picture.GetShapeCount(), 1);
	const Shape& shape = picture.GetShape("e");
	EXPECT_EQ(shape.GetType(), "ellipse");
	EXPECT_EQ(shape.GetBounds(), (Rect{ 1, 2, 30, 40 }));
	EXPECT_EQ(shape.GetColor(), (Color{ 10, 20, 30 }));
}

TEST_F(CommandUndoTest, UndoDeleteKeepsShapeOrder)
{
	AddThree();

	for (const char* id : { "a", "b", "c" })
	{
		history.Execute(std::make_unique<DeleteShapeCommand>(picture, id));
		history.Undo();
		EXPECT_EQ(GetIds(picture), (std::vector<std::string>{ "a", "b", "c" })) << id;
	}
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
	picture.AddShape(CreateShape("rectangle", "r", Color{ 255, 0, 16 }, { 10, 20, 100, 50 }));
	picture.AddShape(CreateShape("ellipse", "e", Color{ 0, 128, 0 }, { 200.5, 200, 80, 40 }));
	picture.AddShape(CreateShape("triangle", "t", Color{ 1, 2, 3 }, { 300, 100, 60, 90 }));

	std::stringstream stream;
	SaveService::Save(picture, stream);
	const auto loaded = LoadService::Load(stream);

	ASSERT_EQ(static_cast<int>(loaded.size()), picture.GetShapeCount());
	for (int i = 0; i < picture.GetShapeCount(); i++)
	{
		EXPECT_EQ(loaded[i]->GetType(), picture.GetShapeAt(i).GetType());
		EXPECT_EQ(loaded[i]->GetBounds(), picture.GetShapeAt(i).GetBounds());
		EXPECT_EQ(loaded[i]->GetColor(), picture.GetShapeAt(i).GetColor());
	}
}
