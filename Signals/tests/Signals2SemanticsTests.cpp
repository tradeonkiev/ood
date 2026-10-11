// Фиксация фактической семантики boost::signals2, на которую опирается Picture/Shape
#include <boost/signals2/connection.hpp>
#include <boost/signals2/shared_connection_block.hpp>
#include <boost/signals2/signal.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
namespace bs2 = boost::signals2;
using Signal = bs2::signal<void(int)>;
} // namespace

TEST(Signals2Semantics, SlotsAreCalledInConnectionOrder)
{
	Signal signal;
	std::string calls;
	bs2::scoped_connection a = signal.connect([&](int) { calls += 'a'; });
	bs2::scoped_connection b = signal.connect([&](int) { calls += 'b'; });
	bs2::scoped_connection c = signal.connect([&](int) { calls += 'c'; });

	signal(0);

	EXPECT_EQ(calls, "abc");
}

TEST(Signals2Semantics, SlotDisconnectingItselfDoesNotAffectOthers)
{
	Signal signal;
	std::string calls;
	bs2::connection self;
	bs2::scoped_connection a = signal.connect([&](int) { calls += 'a'; });
	self = signal.connect([&](int) {
		calls += 'b';
		self.disconnect();
	});
	bs2::scoped_connection c = signal.connect([&](int) { calls += 'c'; });

	signal(0);
	signal(0);

	EXPECT_EQ(calls, "abcac");
}

TEST(Signals2Semantics, SlotDisconnectedDuringEmissionBeforeItsCallIsNotCalled)
{
	Signal signal;
	std::string calls;
	bs2::scoped_connection b;
	bs2::scoped_connection a = signal.connect([&](int) {
		calls += 'a';
		b.disconnect();
	});
	b = signal.connect([&](int) { calls += 'b'; });
	bs2::scoped_connection c = signal.connect([&](int) { calls += 'c'; });

	signal(0);

	EXPECT_EQ(calls, "ac");
}

TEST(Signals2Semantics, SlotConnectedDuringEmissionIsCalledFromNextEmission)
{
	Signal signal;
	std::string calls;
	bs2::scoped_connection added;
	bs2::scoped_connection a = signal.connect([&](int) {
		calls += 'a';
		if (!added.connected())
		{
			added = signal.connect([&](int) { calls += 'd'; });
		}
	});
	bs2::scoped_connection b = signal.connect([&](int) { calls += 'b'; });

	signal(0);
	EXPECT_EQ(calls, "ab");

	signal(0);
	EXPECT_EQ(calls, "ababd");
}

TEST(Signals2Semantics, ScopedConnectionDisconnectsOnDestruction)
{
	Signal signal;
	int calls = 0;
	{
		bs2::scoped_connection connection = signal.connect([&](int) { ++calls; });
		signal(0);
	}
	signal(0);

	EXPECT_EQ(calls, 1);
}

TEST(Signals2Semantics, PlainConnectionKeepsSlotAfterDestruction)
{
	Signal signal;
	int calls = 0;
	{
		bs2::connection connection = signal.connect([&](int) { ++calls; });
	}
	signal(0);

	EXPECT_EQ(calls, 1);
}

TEST(Signals2Semantics, ScopedConnectionCanBeMovedWithoutDisconnecting)
{
	Signal signal;
	int calls = 0;
	std::vector<bs2::scoped_connection> connections;
	{
		bs2::scoped_connection connection = signal.connect([&](int) { ++calls; });
		connections.push_back(std::move(connection));
	}
	signal(0);
	connections.clear();
	signal(0);

	EXPECT_EQ(calls, 1);
}

TEST(Signals2Semantics, ConnectionOutlivingSignalIsSafe)
{
	bs2::connection connection;
	{
		Signal signal;
		connection = signal.connect([](int) {});
		EXPECT_TRUE(connection.connected());
	}

	EXPECT_FALSE(connection.connected());
	EXPECT_NO_THROW(connection.disconnect());
}

TEST(Signals2Semantics, TrackedSlotIsDisconnectedWhenSubscriberDies)
{
	struct Subscriber
	{
		int calls = 0;
		void OnEvent(int) { ++calls; }
	};

	Signal signal;
	auto subscriber = std::make_shared<Subscriber>();
	std::weak_ptr<Subscriber> weak = subscriber;
	bs2::connection connection = signal.connect(
		Signal::slot_type([raw = subscriber.get()](int value) { raw->OnEvent(value); }).track_foreign(weak));

	signal(0);
	EXPECT_EQ(subscriber->calls, 1);

	subscriber.reset();
	EXPECT_NO_THROW(signal(0));
	EXPECT_FALSE(connection.connected());
}

TEST(Signals2Semantics, ExceptionInSlotStopsEmissionAndPropagates)
{
	Signal signal;
	std::string calls;
	bs2::scoped_connection a = signal.connect([&](int) {
		calls += 'a';
		throw std::runtime_error("fail");
	});
	bs2::scoped_connection b = signal.connect([&](int) { calls += 'b'; });

	EXPECT_THROW(signal(0), std::runtime_error);
	EXPECT_EQ(calls, "a");
}

TEST(Signals2Semantics, BlockedConnectionIsSkippedTemporarily)
{
	Signal signal;
	int calls = 0;
	bs2::scoped_connection connection = signal.connect([&](int) { ++calls; });
	{
		bs2::shared_connection_block block(connection);
		signal(0);
	}
	signal(0);

	EXPECT_EQ(calls, 1);
}
