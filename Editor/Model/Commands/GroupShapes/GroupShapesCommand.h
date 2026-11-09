#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <string>
#include <vector>

namespace editor
{

class GroupShapesCommand : public ICommand
{
public:
	GroupShapesCommand(Picture& picture, std::string groupId, std::vector<std::string> ids);
	void Execute() override;
	void Unexecute() override;
	std::string GetName() const override;
	std::vector<std::string> GetShapeIds() const override;

private:
	Picture& m_picture;
	std::string m_groupId;
	std::vector<std::string> m_ids;
	std::vector<int> m_indices;
};

} // namespace editor
