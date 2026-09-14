#ifndef OOD_FUNCTIONAL_MALLARD_DUCK_H
#define OOD_FUNCTIONAL_MALLARD_DUCK_H

#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"

namespace functional {

    class MallardDuck : public FuncDuck {
    public:
        MallardDuck() :
            FuncDuck(FlyBehavior::MakeFlyWithWings(), FunctionalQuackBehavior::MakeQuack(),
                     DanceBehavior::MakeWaltz()) {}

        void Display() const override { std::cout << "I'm mallard duck" << std::endl; }
    };

} // namespace functional

#endif // OOD_FUNCTIONAL_MALLARD_DUCK_H
