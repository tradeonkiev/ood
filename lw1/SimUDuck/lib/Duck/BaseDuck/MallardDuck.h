#ifndef MALLARDDUCK_H
#define MALLARDDUCK_H

#include "../Dance/WaltzDance.h"
#include "../Fly/FlyWithWings.h"
#include "../Quack/QuackBehavior.h"
#include "Duck.h"

class MallardDuck : public Duck {
public:
    MallardDuck() :
        Duck(std::make_unique<FlyWithWings>(), std::make_unique<QuackBehavior>(), std::make_unique<WaltzDance>()) {}

    void Display() const override { std::cout << "I'm mallard duck" << std::endl; }
};

#endif
