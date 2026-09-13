//
// Created by trade on 06.09.2026.
//

#ifndef LW1_MINETDANCE_H
#define LW1_MINETDANCE_H

#include "IDanceBehavior.h"
#include <iostream>

class MinuetDance : public IDanceBehavior
{
public:
	void Dance() override
	{
		std::cout << "Minuet Dance!" << std::endl;
	}
};

#endif // LW1_MINETDANCE_H
