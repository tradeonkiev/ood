#include "Observer/IObserver.h"
#include "Picture/Picture.h"
#include "Shape/Shape.h"
#include "Strategies/IShapeGeometry.h"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
    template<typename Subject>
    class CountingObserver : public shapes::observer::IObserver<Subject> {
    public:
        void Update(const Subject &) override { ++m_updateCount; }

        unsigned GetUpdateCount() const { return m_updateCount; }
        void Reset() { m_updateCount = 0; }

    private:
        unsigned m_updateCount = 0;
    };

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

    std::unique_ptr<shapes::Shape> MakeShape(std::string id) {
        return std::make_unique<shapes::Shape>(std::move(id), gfx::Color{}, std::make_unique<StubShapeGeometry>());
    }
} // namespace

TEST(ObserverTest, ShapeObserverIsNotifiedAfterShapeChanges) {
    auto shape = MakeShape("shape");
    CountingObserver<shapes::Shape> observer;
    shape->RegisterObserver(observer);

    shape->SetColor({10, 20, 30});

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, PictureObserverIsNotifiedWhenOwnedShapeChanges) {
    shapes::Picture picture;
    picture.AddShape(MakeShape("shape"));
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);

    picture.Move(5, -3);

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, PictureObserverIsNotifiedWhenShapeChangesThroughReference) {
    shapes::Picture picture;
    picture.AddShape(MakeShape("shape"));
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);

    shapes::Shape &shape = picture.GetShape("shape");
    shape.SetColor({10, 20, 30});

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, PictureObserverIsNotifiedAfterAddingAndDeletingShape) {
    shapes::Picture picture;
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);

    picture.AddShape(MakeShape("shape"));
    auto removedShape = picture.DeleteShape("shape");

    EXPECT_EQ(observer.GetUpdateCount(), 2);
}

TEST(ObserverTest, PictureStartsObservingAddedShape) {
    shapes::Picture picture;
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);
    picture.AddShape(MakeShape("shape"));
    observer.Reset();

    picture.GetShape("shape").Move(5, -3);

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, PictureStopsObservingDeletedShape) {
    shapes::Picture picture;
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);
    picture.AddShape(MakeShape("shape"));

    auto removedShape = picture.DeleteShape("shape");
    observer.Reset();
    removedShape->SetColor({10, 20, 30});

    EXPECT_EQ(observer.GetUpdateCount(), 0);
}

TEST(ObserverTest, AllObserversAreNotified) {
    auto shape = MakeShape("shape");
    CountingObserver<shapes::Shape> firstObserver;
    CountingObserver<shapes::Shape> secondObserver;
    shape->RegisterObserver(firstObserver);
    shape->RegisterObserver(secondObserver);

    shape->Move(5, -3);

    EXPECT_EQ(firstObserver.GetUpdateCount(), 1);
    EXPECT_EQ(secondObserver.GetUpdateCount(), 1);
}

TEST(ObserverTest, UnsubscribedObserverIsNotNotified) {
    auto shape = MakeShape("shape");
    CountingObserver<shapes::Shape> observer;
    shape->RegisterObserver(observer);
    shape->RemoveObserver(observer);

    shape->SetColor({10, 20, 30});

    EXPECT_EQ(observer.GetUpdateCount(), 0);
}

TEST(ObserverTest, RegisteringSameObserverTwiceDoesNotDuplicateNotifications) {
    auto shape = MakeShape("shape");
    CountingObserver<shapes::Shape> observer;

    EXPECT_TRUE(shape->RegisterObserver(observer));
    EXPECT_FALSE(shape->RegisterObserver(observer));

    shape->SetColor({10, 20, 30});

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, FailedOperationDoesNotNotifyPictureObserver) {
    shapes::Picture picture;
    picture.AddShape(MakeShape("shape"));
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);

    EXPECT_THROW(picture.AddShape(MakeShape("shape")), std::invalid_argument);
    EXPECT_THROW(picture.DeleteShape("missing"), std::out_of_range);

    EXPECT_EQ(picture.GetShapeCount(), 1);
    EXPECT_EQ(observer.GetUpdateCount(), 0);
}
