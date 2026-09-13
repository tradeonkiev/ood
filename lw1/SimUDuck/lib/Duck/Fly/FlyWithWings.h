#ifndef FLYWITHWINGS_H
#define FLYWITHWINGS_H

#include <iostream>
#include "IFlyBehavior.h"

class FlyWithWings : public IFlyBehavior {
public:
    void Fly() override {
        std::cout << "I'm flying with wings!!" << std::endl;
        m_flightCount += 1;
    }

    bool CanFly() override { return true; }

    unsigned GetFlightCount() override { return m_flightCount; }

private:
    unsigned m_flightCount = 0;
};

#endif
