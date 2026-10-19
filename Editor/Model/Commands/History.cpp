#include "History.h"

#include <utility>

namespace ebator
{

void History::Execute(std::unique_ptr<ICommand> command)
{
	command->Execute();
	m_commands.resize(m_nextIndex);
	m_commands.push_back(std::move(command));
	++m_nextIndex;
}

bool History::CanUndo() const { return m_nextIndex > 0; }

bool History::CanRedo() const { return m_nextIndex < m_commands.size(); }

void History::Undo()
{
	if (!CanUndo())
	{
		return;
	}
	m_commands[m_nextIndex - 1]->Unexecute();
	--m_nextIndex;
}

void History::Redo()
{
	if (!CanRedo())
	{
		return;
	}
	m_commands[m_nextIndex]->Execute();
	++m_nextIndex;
}

} // namespace ebator
