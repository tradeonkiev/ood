#ifndef RUBBERDUCK_H
#define RUBBERDUCK_H

#include <iostream>
#include "../Dance/NoDance.h"
#include "../Fly/FlyNoWay.h"
#include "../Quack/SqueakBehavior.h"
#include "Duck.h"

class RubberDuck : public Duck {
public:
    RubberDuck() :
        Duck(std::make_unique<FlyNoWay>(), std::make_unique<SqueakBehavior>(), std::make_unique<NoDance>()) {}

    void Display() const override { std::cout << "I'm rubber duck" << std::endl; }
};

#endif
