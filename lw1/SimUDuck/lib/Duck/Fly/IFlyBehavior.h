#ifndef LAB01_SIMUDUCK_IFLYBEHAVIOR_H
#define LAB01_SIMUDUCK_IFLYBEHAVIOR_H

struct IFlyBehavior {
public:
    virtual ~IFlyBehavior() = default;
    virtual void Fly() = 0;
    virtual bool CanFly() = 0;
    virtual unsigned GetFlightCount() = 0;
};

#endif
