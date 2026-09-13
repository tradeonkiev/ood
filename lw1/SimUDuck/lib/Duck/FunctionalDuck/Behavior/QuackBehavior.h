#ifndef OOD_FUNCTIONAL_QUACK_BEHAVIOR_H
#define OOD_FUNCTIONAL_QUACK_BEHAVIOR_H

#include <functional>
#include <iostream>

namespace FunctionalQuackBehavior {
    using Strategy = std::function<void()>;

    inline Strategy MakeQuack() {
        return [] { std::cout << "Quack Quack!!!" << std::endl; };
    }

    inline Strategy MakeSqueak() {
        return [] { std::cout << "Squeak!!!" << std::endl; };
    }

    inline Strategy MakeMute() {
        return [] {};
    }
} // namespace FunctionalQuackBehavior

#endif // OOD_FUNCTIONAL_QUACK_BEHAVIOR_H
