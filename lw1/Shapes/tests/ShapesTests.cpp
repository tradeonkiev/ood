#include "Canvas/ICanvas.h"
#include "Picture/Picture.h"
#include "Shape/Shape.h"
#include "Strategies/IShapeGeometry.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
    class MockCanvas : public gfx::ICanvas {
    public:
        MOCK_METHOD(void, Clear, (), (override));
        MOCK_METHOD(void, SetColor, (gfx::Color color), (override));
        MOCK_METHOD(void, MoveTo, (shapes::Point point), (override));
        MOCK_METHOD(void, LineTo, (shapes::Point point), (override));
        MOCK_METHOD(void, DrawEllipse, (shapes::Point center, double radiusX, double radiusY), (override));
        MOCK_METHOD(void, DrawText, (shapes::Point point, double fontSize, const std::string &text), (override));
        MOCK_METHOD(void, Flush, (), (const, override));
    };

    class MockShapeGeometry : public shapes::IShapeGeometry {
    public:
        MOCK_METHOD(void, Draw, (gfx::ICanvas & canvas, gfx::Color color), (const, override));
        MOCK_METHOD(void, Move, (int dx, int dy), (override));
        MOCK_METHOD(shapes::Rect, GetBounds, (), (const, override));
        MOCK_METHOD(void, SetBounds, (const shapes::Rect &bounds), (override));
        MOCK_METHOD(std::string, GetType, (), (const, override));
        MOCK_METHOD(std::string, GetParameters, (), (const, override));
    };

    struct ShapeWithMockGeometry {
        std::unique_ptr<shapes::Shape> shape;
        MockShapeGeometry *geometry;
    };

    ShapeWithMockGeometry MakeShape(std::string id, gfx::Color color = {}) {
        auto geometry = std::make_unique<MockShapeGeometry>();
        auto *geometryMock = geometry.get();
        auto shape = std::make_unique<shapes::Shape>(std::move(id), color, std::move(geometry));
        return {std::move(shape), geometryMock};
    }
} // namespace

TEST(ShapeTest, CheckIDAndColor) {
    const gfx::Color color{10, 20, 30};
    auto item = MakeShape("shape", color);

    EXPECT_EQ(item.shape->GetId(), "shape");
    EXPECT_EQ(item.shape->GetColor(), color);
}

TEST(ShapeTest, ChangeColor) {
    auto item = MakeShape("shape");
    const gfx::Color newColor{100, 150, 200};

    item.shape->SetColor(newColor);

    EXPECT_EQ(item.shape->GetColor(), newColor);
}

TEST(ShapeTest, TryToColorDraw) {
    const gfx::Color color{10, 20, 30};
    auto item = MakeShape("shape", color);
    MockCanvas canvas;

    EXPECT_CALL(*item.geometry, Draw(testing::Ref(canvas), color));

    item.shape->Draw(canvas);
}

TEST(ShapeTest, MovesGeometry) {
    auto item = MakeShape("shape");

    EXPECT_CALL(*item.geometry, Move(5, -3));

    item.shape->Move(5, -3);
}

TEST(ShapeTest, ChangesGeometryBounds) {
    auto item = MakeShape("shape");
    const shapes::Rect bounds{10, 20, 100, 50};

    EXPECT_CALL(*item.geometry, SetBounds(bounds));

    item.shape->SetBounds(bounds);
}

TEST(ShapeTest, GetInfoFromGeometry) {
    auto item = MakeShape("shape");

    EXPECT_CALL(*item.geometry, GetType()).WillOnce(testing::Return("rectangle"));
    EXPECT_CALL(*item.geometry, GetParameters()).WillOnce(testing::Return("10 20 100 50"));

    EXPECT_EQ(item.shape->GetType(), "rectangle");
    EXPECT_EQ(item.shape->GetParameters(), "10 20 100 50");
}

TEST(ShapeTest, TryToChangeStrategy) {
    auto item = MakeShape("shape");
    auto newGeometry = std::make_unique<MockShapeGeometry>();
    auto *newGeometryMock = newGeometry.get();

    item.shape->SetStrategy(std::move(newGeometry));

    EXPECT_CALL(*newGeometryMock, GetType()).WillOnce(testing::Return("ellipse"));
    EXPECT_EQ(item.shape->GetType(), "ellipse");
}

TEST(PictureTest, StartEmpty) {
    const shapes::Picture picture;

    EXPECT_EQ(picture.GetShapeCount(), 0);
}

TEST(PictureTest, AddsAndReturnsShapes) {
    shapes::Picture picture;
    auto first = MakeShape("first");
    auto second = MakeShape("second");

    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));

    ASSERT_EQ(picture.GetShapeCount(), 2);
    EXPECT_EQ(picture.GetShape("first").GetId(), "first");
    EXPECT_EQ(picture.GetShape("second").GetId(), "second");
    EXPECT_EQ(picture.GetShapeAt(0).GetId(), "first");
    EXPECT_EQ(picture.GetShapeAt(1).GetId(), "second");
}

TEST(PictureTest, DuplicatedId) {
    shapes::Picture picture;
    auto first = MakeShape("same-id");
    auto second = MakeShape("same-id");
    picture.AddShape(std::move(first.shape));

    EXPECT_THROW(picture.AddShape(std::move(second.shape)), std::invalid_argument);
    EXPECT_EQ(picture.GetShapeCount(), 1);
}

TEST(PictureTest, DeleteShape) {
    shapes::Picture picture;
    auto first = MakeShape("first");
    auto second = MakeShape("second");
    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));

    picture.DeleteShape("first");

    ASSERT_EQ(picture.GetShapeCount(), 1);
    EXPECT_EQ(picture.GetShapeAt(0).GetId(), "second");
    EXPECT_THROW(picture.GetShape("first"), std::out_of_range);
}

TEST(PictureTest, ThrowsWhenShapeDoesNotExist) {
    shapes::Picture picture;

    EXPECT_THROW(picture.GetShape("missing"), std::out_of_range);
    EXPECT_THROW(picture.DeleteShape("missing"), std::out_of_range);
}

TEST(PictureTest, IndexOutOfRange) {
    const shapes::Picture picture;

    EXPECT_THROW(picture.GetShapeAt(0), std::out_of_range);
}

TEST(PictureTest, MovesEveryShape) {
    shapes::Picture picture;
    auto first = MakeShape("first");
    auto second = MakeShape("second");
    auto *firstGeometry = first.geometry;
    auto *secondGeometry = second.geometry;
    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));

    EXPECT_CALL(*firstGeometry, Move(5, -3));
    EXPECT_CALL(*secondGeometry, Move(5, -3));

    picture.Move(5, -3);
}

TEST(PictureTest, ClearsCanvasAndDrawsShapesInOrder) {
    shapes::Picture picture;
    const gfx::Color firstColor{10, 20, 30};
    const gfx::Color secondColor{40, 50, 60};
    auto first = MakeShape("first", firstColor);
    auto second = MakeShape("second", secondColor);
    auto *firstGeometry = first.geometry;
    auto *secondGeometry = second.geometry;
    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));
    MockCanvas canvas;

    testing::InSequence sequence;
    EXPECT_CALL(canvas, Clear());
    EXPECT_CALL(*firstGeometry, Draw(testing::Ref(canvas), firstColor));
    EXPECT_CALL(*secondGeometry, Draw(testing::Ref(canvas), secondColor));

    picture.Draw(canvas);
}

TEST(PictureTest, ClearCanvasWhenCanvasisEmpty) {
    const shapes::Picture picture;
    MockCanvas canvas;

    EXPECT_CALL(canvas, Clear());

    picture.Draw(canvas);
}
