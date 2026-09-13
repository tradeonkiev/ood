#ifndef OOD_FUNCTIONAL_MODEL_DUCK_H
#define OOD_FUNCTIONAL_MODEL_DUCK_H

#include "Behavior/DanceBehavior.h"
#include "Behavior/FlyBehavior.h"
#include "Behavior/QuackBehavior.h"
#include "Duck.h"

class ModelDuck : public Duck
{
public:
	ModelDuck()
		: Duck(FlyBehavior::MakeFlyNoWay(), FunctionalQuackBehavior::MakeQuack(), DanceBehavior::MakeNoDance())
	{
	}

	void Display() const override
	{
		std::cout << "I'm model duck" << std::endl;
	}
};

#endif // OOD_FUNCTIONAL_MODEL_DUCK_H
