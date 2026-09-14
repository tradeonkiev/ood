//
// Created by trade on 06.09.2026.
//

#ifndef IDANCEBEHAVIOR_H
#define IDANCEBEHAVIOR_H

struct IDanceBehavior {
    virtual ~IDanceBehavior() = default;
    virtual void Dance() = 0;
};

#endif
