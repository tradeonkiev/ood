#include "Observer/IObserver.h"
#include "Picture/Picture.h"
#include "Shape/Shape.h"
#include "Strategies/IShapeGeometry.h"

#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
    using Logs = std::vector<std::string>;

    template<typename Subject>
    class LoggingObserver : public shapes::observer::IObserver<Subject> {
    public:
        LoggingObserver(std::string name, Logs &log) : m_name(std::move(name)), m_log(log) {}

        void SetOnceAction(std::function<void()> action) { m_action = std::move(action); }

        void Update() override {
            m_log.push_back(m_name);
            if (m_action) {
                auto action = std::move(m_action);
                m_action = nullptr;
                action();
            }
        }

    private:
        std::string m_name;
        Logs &m_log;
        std::function<void()> m_action;
    };

    template<typename Subject>
    class CountingObserver : public shapes::observer::IObserver<Subject> {
    public:
        void Update() override { ++m_updateCount; }

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
    };

    class SubscriptionShapeFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            shape = MakeShape("shape");
            subA = shape->Subscribe(a);
            subB = shape->Subscribe(b);
            subC = shape->Subscribe(c);
        }

        std::unique_ptr<shapes::Shape> shape;
        Logs log;
        LoggingObserver<shapes::Shape> a{"A", log}, b{"B", log}, c{"C", log};
        shapes::observer::Subscription subA, subB, subC;
    };

    // class SubscriptionShapeFixture : public ::testing::Test {
    // protected:
    //     void SetUp() override {
    //         shape = MakeShape("shape");
    //         shape->RegisterObserver(a);
    //         shape->RegisterObserver(b);
    //         shape->RegisterObserver(c);
    //     }

    //     std::unique_ptr<shapes::Shape> shape;
    //     Logs log;
    //     LoggingObserver<shapes::Shape> a{"A", log}, b{"B", log}, c{"C", log};
    // };
} // namespace

TEST(ObserverTest, NotifyAfterChange) {
    auto shape = MakeShape("shape");
    CountingObserver<shapes::Shape> observer;
    shape->RegisterObserver(observer);

    shape->SetColor({10, 20, 30});

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, PictureObserverIsNotifiedByChangingShape) {
    shapes::Picture picture;
    picture.AddShape(MakeShape("shape"));
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);

    picture.Move(5, -3);

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, NotifyByReference) {
    shapes::Picture picture;
    picture.AddShape(MakeShape("shape"));
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);

    shapes::Shape &shape = picture.GetShape("shape");
    shape.SetColor({10, 20, 30});

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, DeleteNotifingShape) {
    shapes::Picture picture;
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);

    picture.AddShape(MakeShape("shape"));
    auto removedShape = picture.DeleteShape("shape");

    EXPECT_EQ(observer.GetUpdateCount(), 2);
}

TEST(ObserverTest, ObserveAfterAdd) {
    shapes::Picture picture;
    CountingObserver<shapes::Picture> observer;
    picture.RegisterObserver(observer);
    picture.AddShape(MakeShape("shape"));
    observer.Reset();

    picture.GetShape("shape").Move(5, -3);

    EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, AfterDeathNoSubscription) {
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

    EXPECT_NO_THROW(shape->RegisterObserver(observer));
    EXPECT_THROW(shape->RegisterObserver(observer), std::invalid_argument);

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

TEST_F(SubscriptionShapeFixture, UnsubscribeYourselfDuringNotification) {
    b.SetOnceAction([&] { subB.Disconnect(); });

    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "B", "C"}));

    log.clear();
    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "C"}));
}

TEST_F(SubscriptionShapeFixture, ObserverAUnscribesObserverBDuringNotification) {
    a.SetOnceAction([&] { subB.Disconnect(); });

    shape->Move(0, 0);

    EXPECT_EQ(log, (Logs{"A", "C"}));
}

TEST_F(SubscriptionShapeFixture, UnsubscribeOneObserverDosntAffectOtherObservers) {
    b.SetOnceAction([&] { subC.Disconnect(); });

    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "B"}));

    log.clear();
    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "B"}));
}

// ObserverRegisteredDuringNotificationReceives
TEST_F(SubscriptionShapeFixture, ObserverRegisteredDuringNotification) {
    shape->RemoveObserver(c);
    a.SetOnceAction([&] { subC = shape->Subscribe(c); });

    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "B"}));

    log.clear();
    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "B", "C"}));
}

TEST_F(SubscriptionShapeFixture, ObserverChangePositionInNotification) {
    b.SetOnceAction([&] {
        subB.Disconnect();
        subB = shape->Subscribe(b);
    });

    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "B", "C"}));

    log.clear();
    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "C", "B"}));
}

TEST_F(SubscriptionShapeFixture, Observer2RegisteredDuringNotificationReceives) {
    a.SetOnceAction([&] {
        subB.Disconnect();
        subB = shape->Subscribe(b);
    });

    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "C"}));

    log.clear();
    shape->Move(0, 0);
    EXPECT_EQ(log, (Logs{"A", "C", "B"}));
}
