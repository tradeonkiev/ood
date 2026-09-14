#ifndef MODELDUCK_H
#define MODELDUCK_H

#include "../Dance/NoDance.h"
#include "../Fly/FlyNoWay.h"
#include "../Quack/QuackBehavior.h"
#include "Duck.h"

class ModelDuck : public Duck {
public:
    ModelDuck() : Duck(std::make_unique<FlyNoWay>(), std::make_unique<QuackBehavior>(), std::make_unique<NoDance>()) {}

    void Display() const override { std::cout << "I'm model duck" << std::endl; }
};

#endif
