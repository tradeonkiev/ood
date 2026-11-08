#include "GroupCommand.h"

#include <algorithm>
#include <utility>

namespace editor
{

void GroupCommand::Add(std::unique_ptr<ICommand> command) { m_commands.push_back(std::move(command)); }

bool GroupCommand::IsEmpty() const { return m_commands.empty(); }

void GroupCommand::Execute()
{
	for (auto& command : m_commands)
	{
		command->Execute();
	}
}

void GroupCommand::Unexecute()
{
	for (auto it = m_commands.rbegin(); it != m_commands.rend(); it++)
	{
		(*it)->Unexecute();
	}
}

std::string GroupCommand::GetName() const
{
	std::string name = "Group [";
	for (size_t i = 0; i < m_commands.size(); ++i)
	{
		name += (i ? ", " : "") + m_commands[i]->GetName();
	}
	return name + "]";
}

std::vector<std::string> GroupCommand::GetShapeIds() const
{
	std::vector<std::string> ids;
	for (const auto& command : m_commands)
	{
		for (auto& id : command->GetShapeIds())
		{
			if (std::ranges::find(ids, id) == ids.end())
			{
				ids.push_back(std::move(id));
			}
		}
	}
	return ids;
}

} // namespace editor
