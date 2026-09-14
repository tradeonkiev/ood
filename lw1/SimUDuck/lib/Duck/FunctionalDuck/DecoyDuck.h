#ifndef OOD_FUNCTIONAL_DECOY_DUCK_H
#define OOD_FUNCTIONAL_DECOY_DUCK_H

#include <iostream>
#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"

namespace functional {

    class DecoyDuck : public FuncDuck {
    public:
        DecoyDuck() :
            FuncDuck(FlyBehavior::MakeFlyNoWay(), FunctionalQuackBehavior::MakeMute(), DanceBehavior::MakeNoDance()) {}

        void Display() const override { std::cout << "I'm decoy duck" << std::endl; }
    };

} // namespace functional

#endif // OOD_FUNCTIONAL_DECOY_DUCK_H
