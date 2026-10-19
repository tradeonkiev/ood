#pragma once

namespace ebator
{

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
};

} // namespace ebator
