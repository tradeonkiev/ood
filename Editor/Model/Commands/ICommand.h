#pragma once

#include <string>

namespace editor
{

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
	virtual std::string GetName() const = 0;
};

} // namespace editor