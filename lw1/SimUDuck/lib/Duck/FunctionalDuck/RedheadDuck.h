#ifndef OOD_FUNCTIONAL_REDHEAD_DUCK_H
#define OOD_FUNCTIONAL_REDHEAD_DUCK_H

#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"

class RedheadDuck : public Duck {
public:
    RedheadDuck() :
        Duck(FlyBehavior::MakeFlyWithWings(), FunctionalQuackBehavior::MakeQuack(), DanceBehavior::MakeMinuet()) {}

    void Display() const override { std::cout << "I'm redhead duck" << std::endl; }
};

#endif // OOD_FUNCTIONAL_REDHEAD_DUCK_H
