#ifndef DUCKFUNCTIONS_H
#define DUCKFUNCTIONS_H

#include "Duck/BaseDuck/Duck.h"

void DrawDuck(Duck const &duck) { duck.Display(); }

void PlayWithDuck(Duck &duck) {
    DrawDuck(duck);
    duck.Quack();
    duck.Fly();
    duck.Fly();
    std::cout << "Fly count: " << duck.GetFlightCount() << std::endl;
    duck.Dance();
    std::cout << std::endl;
}

#endif
