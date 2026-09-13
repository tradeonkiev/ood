#ifndef DECOYDUCK_H
#define DECOYDUCK_H

#include <iostream>
#include <memory>
#include "../Dance/NoDance.h"
#include "../Fly/FlyNoWay.h"
#include "../Quack/MuteQuackBehavior.h"
#include "Duck.h"

class DecoyDuck : public Duck {
public:
    DecoyDuck() :
        Duck(std::make_unique<FlyNoWay>(), std::make_unique<MuteQuackBehavior>(), std::make_unique<NoDance>()) {}

    void Display() const override { std::cout << "I'm decoy duck" << std::endl; }
};

#endif
