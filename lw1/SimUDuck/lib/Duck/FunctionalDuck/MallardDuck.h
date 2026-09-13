#ifndef OOD_FUNCTIONAL_MALLARD_DUCK_H
#define OOD_FUNCTIONAL_MALLARD_DUCK_H

#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"

class MallardDuck : public Duck
{
public:
	MallardDuck()
		: Duck(FlyBehavior::MakeFlyWithWings(), FunctionalQuackBehavior::MakeQuack(), DanceBehavior::MakeWaltz())
	{
	}

	void Display() const override
	{
		std::cout << "I'm mallard duck" << std::endl;
	}
};

#endif // OOD_FUNCTIONAL_MALLARD_DUCK_H
