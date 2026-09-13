//
// Created by trade on 06.09.2026.
//

#ifndef LW1_WALTZDANCE_H
#define LW1_WALTZDANCE_H

#include "IDanceBehavior.h"
#include <iostream>

class WaltzDance : public IDanceBehavior
{
public:
	void Dance() override
	{
		std::cout << "Waltz Dance!" << std::endl;
	}
};

#endif // LW1_WALTZDANCE_H
