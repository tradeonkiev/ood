//
// Created by trade on 13.09.2026.
//

#ifndef OOD_IDANCEBEHAIVOR_H
#define OOD_IDANCEBEHAIVOR_H

#include <functional>
#include <iostream>

namespace DanceBehavior {
    using Strategy = std::function<void()>;

    inline Strategy MakeWaltz() {
        return [] { std::cout << "Waltz Dance!" << std::endl; };
    }

    inline Strategy MakeMinuet() {
        return [] { std::cout << "Minuet Dance!" << std::endl; };
    }

    inline Strategy MakeNoDance() {
        return [] {};
    }
} // namespace DanceBehavior

#endif // OOD_IDANCEBEHAIVOR_H
