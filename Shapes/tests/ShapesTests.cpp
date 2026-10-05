#include "Observer/IObserver.h"
#include "Picture/Picture.h"
#include "Shape/Shape.h"
#include "Strategies/IShapeGeometry.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{
template <typename Subject>
class CountingObserver : public shapes::observer::IObserver<Subject>
{
public:
	void Update() override { ++m_updateCount; }

	unsigned GetUpdateCount() const { return m_updateCount; }
	void Reset() { m_updateCount = 0; }

private:
	unsigned m_updateCount = 0;
};

class StubShapeGeometry : public shapes::IShapeGeometry
{
public:
	void Draw(gfx::ICanvas&, gfx::Color) const override {}

	void Move(int dx, int dy) override
	{
		m_bounds.left += dx;
		m_bounds.top += dy;
	}

	shapes::Rect GetBounds() const override { return m_bounds; }
	void SetBounds(const shapes::Rect& bounds) override { m_bounds = bounds; }
	std::string GetType() const override { return "stub"; }
	std::string GetParameters() const override { return {}; }

private:
	shapes::Rect m_bounds;
};

std::unique_ptr<shapes::Shape> MakeShape(std::string id)
{
	return std::make_unique<shapes::Shape>(std::move(id), gfx::Color{}, std::make_unique<StubShapeGeometry>());
};

template <typename Subject>
class MockObserver : public shapes::observer::IObserver<Subject>
{
public:
	MOCK_METHOD(void, Update, (), (override));
};

class ABCCreateFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		shape = MakeShape("shape");
		subA = shape->Subscribe(a);
		subB = shape->Subscribe(b);
		subC = shape->Subscribe(c);
	}

	std::unique_ptr<shapes::Shape> shape;
	::testing::StrictMock<MockObserver<shapes::Shape>> a, b, c;
	shapes::observer::Subscription subA, subB, subC;
};
} // namespace

TEST(ObserverTest, NotifyAfterChange)
{
	auto shape = MakeShape("shape");
	CountingObserver<shapes::Shape> observer;
	shape->RegisterObserver(observer);

	shape->SetColor({ 10, 20, 30 });

	EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, PictureObserverIsNotifiedByChangingShape)
{
	shapes::Picture picture;
	picture.AddShape(MakeShape("shape"));
	CountingObserver<shapes::Picture> observer;
	picture.RegisterObserver(observer);

	picture.Move(5, -3);

	EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, NotifyByReference)
{
	shapes::Picture picture;
	picture.AddShape(MakeShape("shape"));
	CountingObserver<shapes::Picture> observer;
	picture.RegisterObserver(observer);

	shapes::Shape& shape = picture.GetShape("shape");
	shape.SetColor({ 10, 20, 30 });

	EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, DeleteNotifingShape)
{
	shapes::Picture picture;
	CountingObserver<shapes::Picture> observer;
	picture.RegisterObserver(observer);

	picture.AddShape(MakeShape("shape"));
	auto removedShape = picture.DeleteShape("shape");

	EXPECT_EQ(observer.GetUpdateCount(), 2);
}

TEST(ObserverTest, ObserveAfterAdd)
{
	shapes::Picture picture;
	CountingObserver<shapes::Picture> observer;
	picture.RegisterObserver(observer);
	picture.AddShape(MakeShape("shape"));
	observer.Reset();

	picture.GetShape("shape").Move(5, -3);

	EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, AfterDeathNoSubscription)
{
	shapes::Picture picture;
	CountingObserver<shapes::Picture> observer;
	picture.RegisterObserver(observer);
	picture.AddShape(MakeShape("shape"));

	auto removedShape = picture.DeleteShape("shape");
	observer.Reset();
	removedShape->SetColor({ 10, 20, 30 });

	EXPECT_EQ(observer.GetUpdateCount(), 0);
}

TEST(ObserverTest, AllObserversAreNotified)
{
	auto shape = MakeShape("shape");
	CountingObserver<shapes::Shape> firstObserver;
	CountingObserver<shapes::Shape> secondObserver;
	shape->RegisterObserver(firstObserver);
	shape->RegisterObserver(secondObserver);

	shape->Move(5, -3);

	EXPECT_EQ(firstObserver.GetUpdateCount(), 1);
	EXPECT_EQ(secondObserver.GetUpdateCount(), 1);
}

TEST(ObserverTest, UnsubscribedObserverIsNotNotified)
{
	auto shape = MakeShape("shape");
	CountingObserver<shapes::Shape> observer;
	shape->RegisterObserver(observer);
	shape->RemoveObserver(observer);

	shape->SetColor({ 10, 20, 30 });

	EXPECT_EQ(observer.GetUpdateCount(), 0);
}

TEST(ObserverTest, RegisteringSameObserverTwiceDoesNotDuplicateNotifications)
{
	auto shape = MakeShape("shape");
	CountingObserver<shapes::Shape> observer;

	EXPECT_NO_THROW(shape->RegisterObserver(observer));
	EXPECT_THROW(shape->RegisterObserver(observer), std::invalid_argument);

	shape->SetColor({ 10, 20, 30 });

	EXPECT_EQ(observer.GetUpdateCount(), 1);
}

TEST(ObserverTest, FailedOperationDoesNotNotifyPictureObserver)
{
	shapes::Picture picture;
	picture.AddShape(MakeShape("shape"));
	CountingObserver<shapes::Picture> observer;
	picture.RegisterObserver(observer);

	EXPECT_THROW(picture.AddShape(MakeShape("shape")), std::invalid_argument);
	EXPECT_THROW(picture.DeleteShape("missing"), std::out_of_range);

	EXPECT_EQ(picture.GetShapeCount(), 1);
	EXPECT_EQ(observer.GetUpdateCount(), 0);
}

TEST_F(ABCCreateFixture, UnsubscribeYourselfDuringNotification)
{
	::testing::InSequence seq;
	EXPECT_CALL(a, Update()).WillOnce([&] { subA.Disconnect(); });
	EXPECT_CALL(b, Update());
	EXPECT_CALL(c, Update());

	EXPECT_CALL(b, Update());
	EXPECT_CALL(c, Update());

	shape->Move(0, 0);
	shape->Move(0, 0);
}

TEST_F(ABCCreateFixture, ObserverAUnsubscribesObserverBDuringNotification)
{
	::testing::InSequence seq;
	EXPECT_CALL(a, Update()).WillOnce([&] { subB.Disconnect(); });
	EXPECT_CALL(c, Update());

	shape->Move(0, 0);
}

TEST_F(ABCCreateFixture, UnsubscribeOneObserverDosntAffectOtherObservers)
{
	::testing::InSequence seq;
	EXPECT_CALL(a, Update());
	EXPECT_CALL(b, Update()).WillOnce([&] { subC.Disconnect(); });

	EXPECT_CALL(a, Update());
	EXPECT_CALL(b, Update());

	shape->Move(0, 0);
	shape->Move(0, 0);
}

TEST_F(ABCCreateFixture, ObserverRegisteredDuringNotification)
{
	shape->RemoveObserver(c);

	::testing::InSequence seq;
	EXPECT_CALL(a, Update()).WillOnce([&] { subC = shape->Subscribe(c); });
	EXPECT_CALL(b, Update());

	EXPECT_CALL(a, Update());
	EXPECT_CALL(b, Update());
	EXPECT_CALL(c, Update());

	shape->Move(0, 0);
	shape->Move(0, 0);
}

TEST_F(ABCCreateFixture, ObserverChangePositionInNotification)
{
	::testing::InSequence seq;
	EXPECT_CALL(a, Update());
	EXPECT_CALL(b, Update()).WillOnce([&] {
		subB.Disconnect();
		subB = shape->Subscribe(b);
	});
	EXPECT_CALL(c, Update());

	EXPECT_CALL(a, Update());
	EXPECT_CALL(c, Update());
	EXPECT_CALL(b, Update());

	shape->Move(0, 0);
	shape->Move(0, 0);
}

TEST_F(ABCCreateFixture, Observer2RegisteredDuringNotificationReceives)
{
	::testing::InSequence seq;
	EXPECT_CALL(a, Update()).WillOnce([&] {
		subB.Disconnect();
		subB = shape->Subscribe(b);
	});
	EXPECT_CALL(c, Update());

	EXPECT_CALL(a, Update());
	EXPECT_CALL(c, Update());
	EXPECT_CALL(b, Update());

	shape->Move(0, 0);
	shape->Move(0, 0);
}
