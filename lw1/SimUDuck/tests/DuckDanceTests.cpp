#include "../lib/Duck/BaseDuck/Duck.h"
#include "../lib/Duck/Dance/NoDance.h"
#include "../lib/Duck/Fly/FlyNoWay.h"
#include "../lib/Duck/Quack/MuteQuackBehavior.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <utility>

namespace {
    class MockDanceBehavior : public IDanceBehavior {
    public:
        MOCK_METHOD(void, Dance, (), (override));
    };

    class TestDuck : public Duck {
    public:
        explicit TestDuck(std::unique_ptr<IDanceBehavior> danceBehavior, std::unique_ptr<IFlyBehavior> flyBehavior,
                          std::unique_ptr<IQuackBehavior> quackBehavior) :
            Duck(std::move(flyBehavior), std::move(quackBehavior), std::move(danceBehavior)) {}

        void Display() const override {}
    };

    class MockFlyBehavior : public IFlyBehavior {
    public:
        MOCK_METHOD(void, Fly, (), (override));
        MOCK_METHOD(bool, CanFly, (), (override));
        MOCK_METHOD(unsigned, GetFlightCount, (), (override));
    };

    class MockQuackBehavior : public IQuackBehavior {
    public:
        MOCK_METHOD(void, Quack, (), (override));
    };
} // namespace

TEST(DuckDanceTest, CallsDanceStrategyExactlyOnce) {
    auto danceBehavior = std::make_unique<MockDanceBehavior>();
    auto *mock = danceBehavior.get();
    TestDuck duck(std::move(danceBehavior), std::make_unique<FlyNoWay>(), std::make_unique<MuteQuackBehavior>());

    EXPECT_CALL(*mock, Dance());
    duck.Dance();
}

TEST(DuckDanceTest, DelegatesEveryDanceCall) {
    auto danceBehavior = std::make_unique<MockDanceBehavior>();
    auto *mock = danceBehavior.get();
    TestDuck duck(std::move(danceBehavior), std::make_unique<FlyNoWay>(), std::make_unique<MuteQuackBehavior>());


    EXPECT_CALL(*mock, Dance()).Times(2);
    duck.Dance();
    duck.Dance();
}

TEST(DuckDanceTest, CallsDance) {
    auto quackBehavior = std::make_unique<MockQuackBehavior>();
    auto flightBehavior = std::make_unique<MockFlyBehavior>();

    unsigned flightCount = 0;
    auto *flightMock = flightBehavior.get();
    auto *quackMock = quackBehavior.get();

    const TestDuck duck(std::make_unique<NoDance>(), std::move(flightBehavior), std::move(quackBehavior));

    ON_CALL(*flightMock, Fly()).WillByDefault([&flightCount] { ++flightCount; });
    ON_CALL(*flightMock, GetFlightCount()).WillByDefault([&flightCount] { return flightCount; });

    EXPECT_CALL(*flightMock, CanFly()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*flightMock, Fly()).Times(2);
    EXPECT_CALL(*flightMock, GetFlightCount()).Times(2);
    EXPECT_CALL(*quackMock, Quack());
    duck.Fly();
    duck.Fly();
}


// TEST(DecoyDuckTest, DoesNotFly) {
//     TestDuck decoyDuck(std::make_unique<NoDance>(), std::make_unique<FlyNoWay>(),
//                        std::make_unique<MuteQuackBehavior>());
//     decoyDuck.Fly();
//     EXPECT_EQ(decoyDuck.GetFlightCount(), 0);
// }

// TEST(DecoyDuckTest, DoesNotQuack) {
//     TestDuck decoyDuck(std::make_unique<NoDance>(), std::make_unique<FlyNoWay>(),
//                        std::make_unique<MuteQuackBehavior>());
//     testing::internal::CaptureStdout();
//     decoyDuck.Quack();
//     std::string output = testing::internal::GetCapturedStdout();
//     EXPECT_TRUE(output);
// }

// TEST(DecoyDuckTest, DoesNotDance) {
//     auto danceBehavior = std::make_unique<MockDanceBehavior>();
//     auto *mock = danceBehavior.get();
//     TestDuck decoyDuck(std::make_unique<NoDance>(), std::make_unique<FlyNoWay>(),
//                        std::make_unique<MuteQuackBehavior>());

//     EXPECT_CALL(*mock, Dance()).Times(0);
//     decoyDuck.Dance();
// }
