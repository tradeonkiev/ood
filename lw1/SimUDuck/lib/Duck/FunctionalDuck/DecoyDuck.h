#ifndef OOD_FUNCTIONAL_DECOY_DUCK_H
#define OOD_FUNCTIONAL_DECOY_DUCK_H

#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"
#include <iostream>

class DecoyDuck : public Duck {
public:
    DecoyDuck() :
        Duck(FlyBehavior::MakeFlyNoWay(), FunctionalQuackBehavior::MakeMute(), DanceBehavior::MakeNoDance()) {}

    void Display() const override { std::cout << "I'm decoy duck" << std::endl; }
};

#endif // OOD_FUNCTIONAL_DECOY_DUCK_H
