#include "History.h"

#include <iostream>
#include <utility>

namespace editor
{

void History::Execute(std::unique_ptr<ICommand> command)
{
	command->Execute();
	std::cout << "[Execute] " << command->GetName() << std::endl;
	m_commands.resize(m_nextIndex);
	m_commands.push_back(std::move(command));
	++m_nextIndex;
}

bool History::CanUndo() const { return m_nextIndex > 0; }

bool History::CanRedo() const { return m_nextIndex < m_commands.size(); }

const ICommand* History::Undo()
{
	if (!CanUndo())
	{
		return nullptr;
	}

	m_commands[m_nextIndex - 1]->Unexecute();
	std::cout << "[Undo] " << m_commands[m_nextIndex - 1]->GetName() << std::endl;
	--m_nextIndex;
	return m_commands[m_nextIndex].get();
}

const ICommand* History::Redo()
{
	if (!CanRedo())
	{
		return nullptr;
	}

	m_commands[m_nextIndex]->Execute();
	std::cout << "[Redo] " << m_commands[m_nextIndex]->GetName() << std::endl;
	++m_nextIndex;
	return m_commands[m_nextIndex - 1].get();
}

} // namespace editor
