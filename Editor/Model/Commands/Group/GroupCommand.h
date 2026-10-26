#pragma once

#include "Model/Commands/ICommand.h"

#include <memory>
#include <vector>

namespace editor
{

class GroupCommand : public ICommand
{
public:
	void Add(std::unique_ptr<ICommand> command);
	bool IsEmpty() const;
	void Execute() override;
	void Unexecute() override;

private:
	std::vector<std::unique_ptr<ICommand>> m_commands;
};

} // namespace editor
