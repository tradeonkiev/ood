#ifndef OOD_FUNCTIONAL_RUBBER_DUCK_H
#define OOD_FUNCTIONAL_RUBBER_DUCK_H

#include <iostream>
#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"

namespace functional {

    class RubberDuck : public FuncDuck {
    public:
        RubberDuck() :
            FuncDuck(FlyBehavior::MakeFlyNoWay(), FunctionalQuackBehavior::MakeSqueak(), DanceBehavior::MakeNoDance()) {
        }

        void Display() const override { std::cout << "I'm rubber duck" << std::endl; }
    };

} // namespace functional

#endif // OOD_FUNCTIONAL_RUBBER_DUCK_H
