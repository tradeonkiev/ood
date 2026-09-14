#ifndef OOD_FUNCTIONAL_MODEL_DUCK_H
#define OOD_FUNCTIONAL_MODEL_DUCK_H

#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"

namespace functional {

class ModelDuck : public FuncDuck {
public:
    ModelDuck() :
        FuncDuck(FlyBehavior::MakeFlyNoWay(), FunctionalQuackBehavior::MakeQuack(), DanceBehavior::MakeNoDance()) {}

    void Display() const override { std::cout << "I'm model duck" << std::endl; }
};

} // namespace functional

#endif // OOD_FUNCTIONAL_MODEL_DUCK_H
