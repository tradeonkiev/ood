//
// Created by trade on 06.09.2026.
//

#ifndef IDANCEBEHAVIOR_H
#define IDANCEBEHAVIOR_H

// TODO: прочитать про удаление ноденс без виртуального деструктора тут
struct IDanceBehavior {
    virtual ~IDanceBehavior() = default;
    virtual void Dance() = 0;
};

#endif
