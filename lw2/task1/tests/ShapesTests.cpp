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
    using testing::InSequence;
    using testing::Ref;
    using testing::Return;

    class MockCanvas final : public gfx::ICanvas {
    public:
        MOCK_METHOD(void, Clear, (), (override));
        MOCK_METHOD(void, SetColor, (gfx::Color color), (override));
        MOCK_METHOD(void, MoveTo, (shapes::Point point), (override));
        MOCK_METHOD(void, LineTo, (shapes::Point point), (override));
        MOCK_METHOD(void, DrawEllipse, (shapes::Point center, double radiusX, double radiusY), (override));
        MOCK_METHOD(void, DrawText, (shapes::Point point, double fontSize, const std::string &text), (override));
        MOCK_METHOD(void, Flush, (), (const, override));
    };

    class MockShapeGeometry final : public shapes::IShapeGeometry {
    public:
        MOCK_METHOD(void, Draw, (gfx::ICanvas & canvas, gfx::Color color), (const, override));
        MOCK_METHOD(void, Move, (int dx, int dy), (override));
        MOCK_METHOD(shapes::Rect, GetBounds, (), (const, override));
        MOCK_METHOD(void, SetBounds, (const shapes::Rect &bounds), (override));
        MOCK_METHOD(std::string, GetType, (), (const, override));
        MOCK_METHOD(std::string, GetParameters, (), (const, override));
    };

    struct ShapeWithMock {
        std::unique_ptr<shapes::Shape> shape;
        MockShapeGeometry *geometry;
    };

    ShapeWithMock MakeShape(std::string id, const gfx::Color color) {
        auto geometry = std::make_unique<MockShapeGeometry>();
        auto *geometryPtr = geometry.get();
        return {std::make_unique<shapes::Shape>(std::move(id), color, std::move(geometry)), geometryPtr};
    }
} // namespace

TEST(ShapeTest, StoresIdAndColor) {
    const gfx::Color color{0x12, 0x34, 0x56};
    auto [shape, geometry] = MakeShape("first", color);

    EXPECT_EQ(shape->GetId(), "first");
    EXPECT_EQ(shape->GetColor(), color);
}

TEST(ShapeTest, ChangesColor) {
    auto [shape, geometry] = MakeShape("first", gfx::Color{});
    const gfx::Color newColor{0xaa, 0xbb, 0xcc};

    shape->SetColor(newColor);

    EXPECT_EQ(shape->GetColor(), newColor);
}

TEST(ShapeTest, DelegatesDrawingToGeometryWithCurrentColorAndCanvas) {
    const gfx::Color color{10, 20, 30};
    auto [shape, geometry] = MakeShape("first", color);
    MockCanvas canvas;

    EXPECT_CALL(*geometry, Draw(Ref(canvas), color));

    shape->Draw(canvas);
}

TEST(ShapeTest, DelegatesMoveToGeometry) {
    auto [shape, geometry] = MakeShape("first", gfx::Color{});

    EXPECT_CALL(*geometry, Move(15, -7));

    shape->Move(15, -7);
}

TEST(ShapeTest, DelegatesBoundsOperationsToGeometry) {
    auto [shape, geometry] = MakeShape("first", gfx::Color{});
    const shapes::Rect oldBounds{1, 2, 30, 40};
    const shapes::Rect newBounds{5, 6, 70, 80};

    EXPECT_CALL(*geometry, GetBounds()).WillOnce(Return(oldBounds));
    EXPECT_CALL(*geometry, SetBounds(newBounds));

    EXPECT_EQ(shape->GetBounds(), oldBounds);
    shape->SetBounds(newBounds);
}

TEST(ShapeTest, DelegatesTypeAndParametersToGeometry) {
    auto [shape, geometry] = MakeShape("first", gfx::Color{});

    EXPECT_CALL(*geometry, GetType()).WillOnce(Return("rectangle"));
    EXPECT_CALL(*geometry, GetParameters()).WillOnce(Return("1 2 30 40"));

    EXPECT_EQ(shape->GetType(), "rectangle");
    EXPECT_EQ(shape->GetParameters(), "1 2 30 40");
}

TEST(ShapeTest, UsesReplacementGeometry) {
    auto [shape, oldGeometry] = MakeShape("first", gfx::Color{});
    auto newGeometry = std::make_unique<MockShapeGeometry>();
    auto *newGeometryPtr = newGeometry.get();

    shape->SetStrategy(std::move(newGeometry));

    EXPECT_CALL(*newGeometryPtr, GetType()).WillOnce(Return("ellipse"));
    EXPECT_EQ(shape->GetType(), "ellipse");
}

TEST(PictureTest, IsEmptyInitially) {
    const shapes::Picture picture;

    EXPECT_EQ(picture.GetShapeCount(), 0);
}

TEST(PictureTest, AddsShapesAndPreservesTheirOrder) {
    shapes::Picture picture;
    auto first = MakeShape("first", gfx::Color{});
    auto second = MakeShape("second", gfx::Color{});
    auto *firstPtr = first.shape.get();
    auto *secondPtr = second.shape.get();

    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));

    ASSERT_EQ(picture.GetShapeCount(), 2);
    EXPECT_EQ(&picture.GetShape("first"), firstPtr);
    EXPECT_EQ(&picture.GetShapeAt(0), firstPtr);
    EXPECT_EQ(&picture.GetShapeAt(1), secondPtr);

    const auto &constPicture = picture;
    EXPECT_EQ(&constPicture.GetShape("second"), secondPtr);
    EXPECT_EQ(&constPicture.GetShapeAt(1), secondPtr);
}

TEST(PictureTest, RejectsDuplicateIdAndKeepsExistingShape) {
    shapes::Picture picture;
    auto first = MakeShape("duplicate", gfx::Color{});
    auto duplicate = MakeShape("duplicate", gfx::Color{1, 2, 3});
    auto *firstPtr = first.shape.get();
    picture.AddShape(std::move(first.shape));

    EXPECT_THROW(picture.AddShape(std::move(duplicate.shape)), std::invalid_argument);
    EXPECT_EQ(picture.GetShapeCount(), 1);
    EXPECT_EQ(&picture.GetShape("duplicate"), firstPtr);
}

TEST(PictureTest, DeletesShapeByIdAndPreservesOrderOfRemainingShapes) {
    shapes::Picture picture;
    auto first = MakeShape("first", gfx::Color{});
    auto second = MakeShape("second", gfx::Color{});
    auto third = MakeShape("third", gfx::Color{});
    auto *firstPtr = first.shape.get();
    auto *thirdPtr = third.shape.get();
    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));
    picture.AddShape(std::move(third.shape));

    picture.DeleteShape("second");

    ASSERT_EQ(picture.GetShapeCount(), 2);
    EXPECT_EQ(&picture.GetShapeAt(0), firstPtr);
    EXPECT_EQ(&picture.GetShapeAt(1), thirdPtr);
}

TEST(PictureTest, ReportsMissingShapeAndInvalidIndex) {
    shapes::Picture picture;
    const auto &constPicture = picture;

    EXPECT_THROW(picture.GetShape("missing"), std::out_of_range);
    EXPECT_THROW(constPicture.GetShape("missing"), std::out_of_range);
    EXPECT_THROW(picture.GetShapeAt(0), std::out_of_range);
    EXPECT_THROW(constPicture.GetShapeAt(0), std::out_of_range);
    EXPECT_THROW(picture.DeleteShape("missing"), std::out_of_range);
}

TEST(PictureTest, MovesEveryShapeAndPreservesItsSize) {
    shapes::Picture picture;
    auto first = MakeShape("first", gfx::Color{});
    auto second = MakeShape("second", gfx::Color{});
    auto *firstGeometry = first.geometry;
    auto *secondGeometry = second.geometry;
    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));

    EXPECT_CALL(*firstGeometry, GetBounds()).WillOnce(Return(shapes::Rect{1, 2, 30, 40}));
    EXPECT_CALL(*firstGeometry, SetBounds(shapes::Rect{6.5, -1, 30, 40}));
    EXPECT_CALL(*secondGeometry, GetBounds()).WillOnce(Return(shapes::Rect{-10, 20, 5, 6}));
    EXPECT_CALL(*secondGeometry, SetBounds(shapes::Rect{-4.5, 17, 5, 6}));

    picture.Move(5.5, -3);
}

TEST(PictureTest, ClearsCanvasThenDrawsEveryShapeInOrder) {
    shapes::Picture picture;
    const gfx::Color firstColor{1, 2, 3};
    const gfx::Color secondColor{4, 5, 6};
    auto first = MakeShape("first", firstColor);
    auto second = MakeShape("second", secondColor);
    auto *firstGeometry = first.geometry;
    auto *secondGeometry = second.geometry;
    picture.AddShape(std::move(first.shape));
    picture.AddShape(std::move(second.shape));
    MockCanvas canvas;

    InSequence sequence;
    EXPECT_CALL(canvas, Clear());
    EXPECT_CALL(*firstGeometry, Draw(Ref(canvas), firstColor));
    EXPECT_CALL(*secondGeometry, Draw(Ref(canvas), secondColor));

    picture.Draw(canvas);
}

TEST(PictureTest, ClearsCanvasWhenThereAreNoShapes) {
    const shapes::Picture picture;
    MockCanvas canvas;

    EXPECT_CALL(canvas, Clear());

    picture.Draw(canvas);
}
