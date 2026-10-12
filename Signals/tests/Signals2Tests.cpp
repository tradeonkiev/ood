#include <boost/signals2/connection.hpp>
#include <boost/signals2/shared_connection_block.hpp>
#include <boost/signals2/signal.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
using Signal = boost::signals2::signal<void(int)>;
using Handler = ::testing::StrictMock<::testing::MockFunction<void(int)>>;
using ::testing::_;
using ::testing::InSequence;

class ABCSignalFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		subA = signal.connect(a.AsStdFunction());
		subB = signal.connect(b.AsStdFunction());
		subC = signal.connect(c.AsStdFunction());
	}

	Signal signal;
	Handler a, b, c;
	boost::signals2::scoped_connection subA, subB, subC;
};
} // namespace

TEST_F(ABCSignalFixture, SlotsAreCalledInConnectionOrder)
{
	InSequence seq;
	EXPECT_CALL(a, Call(1));
	EXPECT_CALL(b, Call(1));
	EXPECT_CALL(c, Call(1));

	signal(1);
}

TEST_F(ABCSignalFixture, SlotDisconnectingItselfDoesNotAffectOthers)
{
	InSequence seq;
	EXPECT_CALL(a, Call(1));
	EXPECT_CALL(b, Call(1)).WillOnce([&] { subB.disconnect(); });
	EXPECT_CALL(c, Call(1));

	EXPECT_CALL(a, Call(2));
	EXPECT_CALL(c, Call(2));

	signal(1);
	signal(2);
}

TEST_F(ABCSignalFixture, SlotDisconnectedDuringEmissionBeforeItsCallIsNotCalled)
{
	InSequence seq;
	EXPECT_CALL(a, Call(1)).WillOnce([&] { subB.disconnect(); });
	EXPECT_CALL(c, Call(1));

	signal(1);
}

TEST_F(ABCSignalFixture, SlotConnectedDuringEmissionIsCalledFromNextEmission)
{
	Handler d;
	boost::signals2::scoped_connection subD;

	InSequence seq;
	EXPECT_CALL(a, Call(1)).WillOnce([&] { subD = signal.connect(d.AsStdFunction()); });
	EXPECT_CALL(b, Call(1));
	EXPECT_CALL(c, Call(1));

	EXPECT_CALL(a, Call(2));
	EXPECT_CALL(b, Call(2));
	EXPECT_CALL(c, Call(2));
	EXPECT_CALL(d, Call(2));

	signal(1);
	signal(2);
}

TEST(Signals2Semantics, ScopedConnectionDisconnectsOnDestruction)
{
	Signal signal;
	Handler handler;
	EXPECT_CALL(handler, Call(1));

	{
		boost::signals2::scoped_connection connection = signal.connect(handler.AsStdFunction());
		signal(1);
	}
	signal(2);
}

TEST(Signals2Semantics, PlainConnectionKeepsSlotAfterDestruction)
{
	Signal signal;
	Handler handler;
	EXPECT_CALL(handler, Call(1));

	{
		boost::signals2::connection connection = signal.connect(handler.AsStdFunction());
	}
	signal(1);
}

TEST(Signals2Semantics, ScopedConnectionCanBeMovedWithoutDisconnecting)
{
	Signal signal;
	Handler handler;
	EXPECT_CALL(handler, Call(1));

	std::vector<boost::signals2::scoped_connection> connections;
	{
		boost::signals2::scoped_connection connection = signal.connect(handler.AsStdFunction());
		connections.push_back(std::move(connection));
	}
	signal(1);
	connections.clear();
	signal(2);
}

TEST(Signals2Semantics, ConnectionOutlivingSignalIsSafe)
{
	Handler handler;
	EXPECT_CALL(handler, Call(_)).Times(0);

	boost::signals2::connection connection;
	{
		Signal signal;
		connection = signal.connect(handler.AsStdFunction());
		EXPECT_TRUE(connection.connected());
	}

	EXPECT_FALSE(connection.connected());
	EXPECT_NO_THROW(connection.disconnect());
}

TEST_F(ABCSignalFixture, ExceptionInSlotStopsEmissionAndPropagates)
{
	EXPECT_CALL(a, Call(1)).WillOnce(::testing::Throw(std::runtime_error("fail")));

	EXPECT_THROW(signal(1), std::runtime_error);
}

TEST(Signals2Semantics, BlockedConnectionIsSkippedTemporarily)
{
	Signal signal;
	Handler handler;
	boost::signals2::scoped_connection connection = signal.connect(handler.AsStdFunction());

	EXPECT_CALL(handler, Call(2));

	{
		boost::signals2::shared_connection_block block(connection);
		signal(1);
	}
	signal(2);
}