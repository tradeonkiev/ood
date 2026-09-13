#ifndef IQUAKBEHAVIOR_H
#define IQUAKBEHAVIOR_H

struct IQuackBehavior {
    virtual ~IQuackBehavior() = default;
    virtual void Quack() = 0;
};

#endif
