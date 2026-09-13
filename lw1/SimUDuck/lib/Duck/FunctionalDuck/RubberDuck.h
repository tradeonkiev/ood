#ifndef OOD_FUNCTIONAL_RUBBER_DUCK_H
#define OOD_FUNCTIONAL_RUBBER_DUCK_H

#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"
#include <iostream>

class RubberDuck : public Duck
{
public:
	RubberDuck()
		: Duck(FlyBehavior::MakeFlyNoWay(), FunctionalQuackBehavior::MakeSqueak(), DanceBehavior::MakeNoDance())
	{
	}

	void Display() const override
	{
		std::cout << "I'm rubber duck" << std::endl;
	}
};

#endif // OOD_FUNCTIONAL_RUBBER_DUCK_H
