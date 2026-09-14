//
// Created by trade on 13.09.2026.
//

#ifndef OOD_FLYBEHAVIOR_H
#define OOD_FLYBEHAVIOR_H
#include <functional>
#include <iostream>

namespace FlyBehavior {
    using Strategy = std::function<unsigned()>;

    inline Strategy MakeFlyWithWings() {
        return [flightCount = 0]() mutable {
            std::cout << "I'm flying with wings!!" << std::endl;
            return ++flightCount;
        };
    }

    inline Strategy MakeFlyNoWay() {
        return [] { return 0; };
    }
} // namespace FlyBehavior

#endif // OOD_FLYBEHAVIOR_H
