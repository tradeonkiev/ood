#include "../lib/Duck/FunctionalDuck/DecoyDuck.h"
#include "../lib/Duck/FunctionalDuck/MallardDuck.h"
#include "../lib/Duck/FunctionalDuck/ModelDuck.h"
#include "../lib/Duck/FunctionalDuck/RedheadDuck.h"
#include "../lib/Duck/FunctionalDuck/RubberDuck.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <type_traits>
#include <utility>

class TestFunctionalDuck : public functional::FuncDuck {
public:
    TestFunctionalDuck(std::function<unsigned()> flyBehavior, std::function<void()> quackBehavior,
                       std::function<void()> danceBehavior) :
        FuncDuck(std::move(flyBehavior), std::move(quackBehavior), std::move(danceBehavior)) {}

    void Display() const override {}
};

TEST(FunctionalDuckTest, DelegatesQuackToBehavior) {
    unsigned quackCount = 0;
    TestFunctionalDuck duck([] { return 0; }, [&quackCount] { ++quackCount; }, [] {});

    duck.Quack();
    duck.Quack();

    EXPECT_EQ(quackCount, 2);
}

TEST(FunctionalDuckTest, DelegatesDanceToBehavior) {
    unsigned danceCount = 0;
    TestFunctionalDuck duck([] { return 0; }, [] {}, [&danceCount] { ++danceCount; });

    duck.Dance();
    duck.Dance();

    EXPECT_EQ(danceCount, 2);
}

TEST(FunctionalDuckTest, StoresFlightCountReturnedByBehavior) {
    unsigned flightCount = 0;
    TestFunctionalDuck duck([&flightCount] { return ++flightCount; }, [] {}, [] {});

    duck.Fly();
    EXPECT_EQ(duck.GetFlightCount(), 1);

    duck.Fly();
    EXPECT_EQ(duck.GetFlightCount(), 2);
}

TEST(FunctionalDuckTest, QuacksAfterEverySecondFlight) {
    unsigned flightCount = 0;
    unsigned quackCount = 0;
    TestFunctionalDuck duck([&flightCount] { return ++flightCount; }, [&quackCount] { ++quackCount; }, [] {});

    duck.Fly();
    EXPECT_EQ(quackCount, 0);

    duck.Fly();
    EXPECT_EQ(quackCount, 1);

    duck.Fly();
    EXPECT_EQ(quackCount, 1);

    duck.Fly();
    EXPECT_EQ(quackCount, 2);
}

TEST(FunctionalDuckTest, NoQuackNoFly) {
    testing::MockFunction<unsigned()> flyBehavior;
    testing::MockFunction<void()> quackBehavior;
    TestFunctionalDuck duck(flyBehavior.AsStdFunction(), quackBehavior.AsStdFunction(), [] {});

    EXPECT_CALL(flyBehavior, Call()).Times(2).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(quackBehavior, Call()).Times(0);

    duck.Fly();
    duck.Fly();

    EXPECT_EQ(duck.GetFlightCount(), 0);
}

TEST(FunctionalDuckTest, ResetsFlightCountWhenFlyBehaviorChanges) {
    unsigned flightCount = 0;
    TestFunctionalDuck duck([&flightCount] { return ++flightCount; }, [] {}, [] {});
    duck.Fly();
    duck.Fly();

    duck.SetFlyBehavior([] { return 5; });

    EXPECT_EQ(duck.GetFlightCount(), 0);
    duck.Fly();
    EXPECT_EQ(duck.GetFlightCount(), 5);
}

TEST(FunctionalDuckTest, UsesNewDanceBehaviorAfterReplacement) {
    unsigned firstDanceCount = 0;
    unsigned secondDanceCount = 0;
    TestFunctionalDuck duck([] { return 0; }, [] {}, [&firstDanceCount] { ++firstDanceCount; });
    duck.Dance();

    duck.SetDanceBehavior([&secondDanceCount] { ++secondDanceCount; });
    duck.Dance();

    EXPECT_EQ(firstDanceCount, 1);
    EXPECT_EQ(secondDanceCount, 1);
}
