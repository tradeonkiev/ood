#pragma once

#include <string>
#include <vector>

namespace editor
{

class ICommand
{
public:
	virtual ~ICommand() = default;
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
	virtual std::string GetName() const = 0;
	virtual std::vector<std::string> GetShapeIds() const { return {}; }
};

} // namespace editor