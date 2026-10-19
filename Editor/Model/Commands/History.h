#pragma once

#include "Model/Commands/ICommand.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace ebator
{

class History
{
public:
	void Execute(std::unique_ptr<ICommand> command);
	bool CanUndo() const;
	bool CanRedo() const;
	void Undo();
	void Redo();

private:
	std::vector<std::unique_ptr<ICommand>> m_commands;
	int m_nextIndex = 0;
};

} // namespace ebator
