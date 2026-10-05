#include "Observer/Subscription.h"
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
    using shapes::Picture;
    using shapes::Shape;
    using shapes::observer::Subscription;
    using ::testing::_;
    using ::testing::InSequence;
    using ::testing::MockFunction;
    using ::testing::Ref;
    using ::testing::StrictMock;

    class StubShapeGeometry : public shapes::IShapeGeometry {
    public:
        void Draw(gfx::ICanvas &, gfx::Color) const override {}

        void Move(int dx, int dy) override {
            m_bounds.left += dx;
            m_bounds.top += dy;
        }

        shapes::Rect GetBounds() const override { return m_bounds; }
        void SetBounds(const shapes::Rect &bounds) override { m_bounds = bounds; }
        std::string GetType() const override { return "stub"; }
        std::string GetParameters() const override { return {}; }

    private:
        shapes::Rect m_bounds;
    };

    std::unique_ptr<Shape> MakeShape(std::string id, gfx::Color color = {}) {
        return std::make_unique<Shape>(std::move(id), color, std::make_unique<StubShapeGeometry>());
    }

    class MockChangeLog {
    public:
        MOCK_METHOD(void, OnShapeAdded, (const Shape &shape));
        MOCK_METHOD(void, OnShapeDeleted, (const Shape &shape));
        MOCK_METHOD(void, OnShapeMoved, (const Shape &shape, int dx, int dy));
        MOCK_METHOD(void, OnShapeColorChanged, (const Shape &shape, gfx::Color oldColor, gfx::Color newColor));
        MOCK_METHOD(void, OnShapeGeometryChanged, (const Shape &shape));
    };

    using ShapeHandler = MockFunction<void(const Shape &)>;
    using MovedHandler = MockFunction<void(const Shape &, int, int)>;
    using ColorHandler = MockFunction<void(const Shape &, gfx::Color, gfx::Color)>;

    class PictureEventsFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            picture.AddShape(MakeShape("shape", oldColor));
            shape = &picture.GetShape("shape");

            addedSub = picture.SubscribeToShapeAdded(added.AsStdFunction());
            deletedSub = picture.SubscribeToShapeRemoved(deleted.AsStdFunction());
            movedSub = picture.SubscribeToShapeMoved(moved.AsStdFunction());
            colorSub = picture.SubscribeToShapeColorChanged(colorChanged.AsStdFunction());
            geometrySub = picture.SubscribeToShapeGeometryChanged(geometryChanged.AsStdFunction());
        }

        const gfx::Color oldColor{1, 2, 3};
        const gfx::Color newColor{10, 20, 30};

        Picture picture;
        Shape *shape = nullptr;

        StrictMock<ShapeHandler> added;
        StrictMock<ShapeHandler> deleted;
        StrictMock<MovedHandler> moved;
        StrictMock<ColorHandler> colorChanged;
        StrictMock<ShapeHandler> geometryChanged;
        Subscription addedSub, deletedSub, movedSub, colorSub, geometrySub;
    };

    class ABCMovedFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            picture.AddShape(MakeShape("shape"));
            shape = &picture.GetShape("shape");

            subA = picture.SubscribeToShapeMoved(a.AsStdFunction());
            subB = picture.SubscribeToShapeMoved(b.AsStdFunction());
            subC = picture.SubscribeToShapeMoved(c.AsStdFunction());
        }

        Picture picture;
        Shape *shape = nullptr;
        StrictMock<MovedHandler> a, b, c;
        Subscription subA, subB, subC;
    };
} // namespace

TEST_F(PictureEventsFixture, Test1ShapeAddedHandlerIsCalledOnAdd) {
    auto newShape = MakeShape("new");
    EXPECT_CALL(added, Call(Ref(*newShape)));

    picture.AddShape(std::move(newShape));
}

TEST_F(PictureEventsFixture, Test2ShapeAddedHandlerIsNotCalledOnMove) {
    EXPECT_CALL(added, Call(_)).Times(0);
    EXPECT_CALL(moved, Call(_, _, _));

    shape->Move(1, 1);
}

TEST_F(PictureEventsFixture, Test3MovedHandlerReceivesShapeAndOffset) {
    EXPECT_CALL(moved, Call(Ref(*shape), 1, 1));

    shape->Move(1, 1);
}

TEST_F(PictureEventsFixture, Test4MovingPictureNotifiesEveryShapeWithOffset) {
    EXPECT_CALL(added, Call(_));
    picture.AddShape(MakeShape("second"));
    Shape &second = picture.GetShape("second");

    InSequence seq;
    EXPECT_CALL(moved, Call(Ref(*shape), 1, 1));
    EXPECT_CALL(moved, Call(Ref(second), 1, 1));

    picture.Move(1, 1);
}

TEST(PictureEventsTest, Test5OneObjectHandlesSeveralEvents) {
    Picture picture;
    StrictMock<MockChangeLog> log;

    auto addedSub = picture.SubscribeToShapeAdded([&log](const Shape &s) { log.OnShapeAdded(s); });
    auto deletedSub = picture.SubscribeToShapeRemoved([&log](const Shape &s) { log.OnShapeDeleted(s); });
    auto movedSub =
            picture.SubscribeToShapeMoved([&log](const Shape &s, int dx, int dy) { log.OnShapeMoved(s, dx, dy); });
    auto colorSub =
            picture.SubscribeToShapeColorChanged([&log](const Shape &s, gfx::Color oldColor, gfx::Color newColor) {
                log.OnShapeColorChanged(s, oldColor, newColor);
            });
    auto geometrySub =
            picture.SubscribeToShapeGeometryChanged([&log](const Shape &s) { log.OnShapeGeometryChanged(s); });

    auto newShape = MakeShape("shape", {1, 2, 3});
    Shape &shape = *newShape;

    InSequence seq;
    EXPECT_CALL(log, OnShapeAdded(Ref(shape)));
    EXPECT_CALL(log, OnShapeMoved(Ref(shape), 1, 2));
    EXPECT_CALL(log, OnShapeColorChanged(Ref(shape), gfx::Color{1, 2, 3}, gfx::Color{4, 5, 6}));
    EXPECT_CALL(log, OnShapeGeometryChanged(Ref(shape)));
    EXPECT_CALL(log, OnShapeDeleted(Ref(shape)));

    picture.AddShape(std::move(newShape));
    shape.Move(1, 2);
    shape.SetColor({4, 5, 6});
    shape.SetBounds({0, 0, 10, 10});
    auto removed = picture.DeleteShape("shape");
}

TEST_F(PictureEventsFixture, SeveralHandlersOnOneEvent) {
    StrictMock<ColorHandler> second;
    auto secondSub = picture.SubscribeToShapeColorChanged(second.AsStdFunction());

    EXPECT_CALL(colorChanged, Call(Ref(*shape), oldColor, newColor));
    EXPECT_CALL(second, Call(Ref(*shape), oldColor, newColor));

    shape->SetColor(newColor);
}

TEST_F(PictureEventsFixture, Test6DisconnectingOneEventDoesNotAffectOthers) {
    movedSub.Disconnect();

    EXPECT_CALL(colorChanged, Call(Ref(*shape), oldColor, newColor));
    EXPECT_CALL(geometryChanged, Call(Ref(*shape)));
    EXPECT_CALL(added, Call(_));

    shape->Move(1, 1);
    shape->SetColor(newColor);
    shape->SetBounds({0, 0, 1, 1});
    picture.AddShape(MakeShape("new"));
}

TEST(PictureEventsTest, Test7SubscriptionIsDisconnectedOnDestructionForEveryEvent) {
    Picture picture;
    StrictMock<ShapeHandler> added;
    StrictMock<ShapeHandler> deleted;
    StrictMock<MovedHandler> moved;
    StrictMock<ColorHandler> colorChanged;
    StrictMock<ShapeHandler> geometryChanged;

    EXPECT_CALL(added, Call(_));
    EXPECT_CALL(deleted, Call(_));
    EXPECT_CALL(moved, Call(_, _, _));
    EXPECT_CALL(colorChanged, Call(_, _, _));
    EXPECT_CALL(geometryChanged, Call(_));

    auto act = [&picture](const std::string &id) {
        picture.AddShape(MakeShape(id));
        Shape &shape = picture.GetShape(id);
        shape.Move(1, 1);
        shape.SetColor({1, 1, 1});
        shape.SetBounds({0, 0, 1, 1});
        auto removed = picture.DeleteShape(id);
    };

    {
        auto addedSub = picture.SubscribeToShapeAdded(added.AsStdFunction());
        auto deletedSub = picture.SubscribeToShapeRemoved(deleted.AsStdFunction());
        auto movedSub = picture.SubscribeToShapeMoved(moved.AsStdFunction());
        auto colorSub = picture.SubscribeToShapeColorChanged(colorChanged.AsStdFunction());
        auto geometrySub = picture.SubscribeToShapeGeometryChanged(geometryChanged.AsStdFunction());

        act("first");
    }

    act("second");
}

TEST(PictureEventsTest, PictureDestroyedBeforeSubscriptionIsSafe) {
    Subscription subscription;
    {
        Picture picture;
        subscription = picture.SubscribeToShapeAdded([](const Shape &) {});
        picture.AddShape(MakeShape("shape"));
    }

    EXPECT_NO_THROW(subscription.Disconnect());
}

TEST_F(ABCMovedFixture, Test8UnsubscribeYourselfDuringNotification) {
    InSequence seq;
    EXPECT_CALL(a, Call(_, 1, 1));
    EXPECT_CALL(b, Call(_, 1, 1)).WillOnce([&] { subB.Disconnect(); });
    EXPECT_CALL(c, Call(_, 1, 1));

    EXPECT_CALL(a, Call(_, 2, 2));
    EXPECT_CALL(c, Call(_, 2, 2));

    shape->Move(1, 1);
    shape->Move(2, 2);
}

TEST_F(ABCMovedFixture, Test8HandlerDisconnectedBeforeItsCallIsNotCalled) {
    InSequence seq;
    EXPECT_CALL(a, Call(_, 1, 1)).WillOnce([&] { subB.Disconnect(); });
    EXPECT_CALL(c, Call(_, 1, 1));

    shape->Move(1, 1);
}

TEST_F(ABCMovedFixture, Test8DisconnectingOneHandlerDoesNotAffectOthers) {
    InSequence seq;
    EXPECT_CALL(a, Call(_, 1, 1));
    EXPECT_CALL(b, Call(_, 1, 1)).WillOnce([&] { subC.Disconnect(); });

    EXPECT_CALL(a, Call(_, 2, 2));
    EXPECT_CALL(b, Call(_, 2, 2));

    shape->Move(1, 1);
    shape->Move(2, 2);
}

TEST_F(ABCMovedFixture, Test9HandlerAddedDuringNotificationIsCalledFromNextEvent) {
    StrictMock<MovedHandler> d;
    Subscription subD;

    InSequence seq;
    EXPECT_CALL(a, Call(_, 1, 1)).WillOnce([&] { subD = picture.SubscribeToShapeMoved(d.AsStdFunction()); });
    EXPECT_CALL(b, Call(_, 1, 1));
    EXPECT_CALL(c, Call(_, 1, 1));

    EXPECT_CALL(a, Call(_, 2, 2));
    EXPECT_CALL(b, Call(_, 2, 2));
    EXPECT_CALL(c, Call(_, 2, 2));
    EXPECT_CALL(d, Call(_, 2, 2));

    shape->Move(1, 1);
    shape->Move(2, 2);
}

TEST_F(ABCMovedFixture, ResubscribedHandlerMovesToEnd) {
    InSequence seq;
    EXPECT_CALL(a, Call(_, 1, 1));
    EXPECT_CALL(b, Call(_, 1, 1)).WillOnce([&] {
        subB.Disconnect();
        subB = picture.SubscribeToShapeMoved(b.AsStdFunction());
    });
    EXPECT_CALL(c, Call(_, 1, 1));

    EXPECT_CALL(a, Call(_, 2, 2));
    EXPECT_CALL(c, Call(_, 2, 2));
    EXPECT_CALL(b, Call(_, 2, 2));

    shape->Move(1, 1);
    shape->Move(2, 2);
}

TEST_F(PictureEventsFixture, Test10ChangingColorThroughShapeReferenceNotifiesPicture) {
    EXPECT_CALL(colorChanged, Call(Ref(*shape), oldColor, newColor));

    picture.GetShape("shape").SetColor(newColor);
}

TEST_F(PictureEventsFixture, DeletedShapeNoLongerNotifiesPicture) {
    EXPECT_CALL(deleted, Call(Ref(*shape)));
    auto removed = picture.DeleteShape("shape");

    removed->Move(1, 1);
    removed->SetColor(newColor);
    removed->SetBounds({1, 2, 3, 4});
}

TEST_F(PictureEventsFixture, FailedOperationsDoNotNotify) {
    EXPECT_THROW(picture.AddShape(MakeShape("shape")), std::invalid_argument);
    EXPECT_THROW(picture.DeleteShape("missing"), std::out_of_range);

    EXPECT_EQ(picture.GetShapeCount(), 1);
}
