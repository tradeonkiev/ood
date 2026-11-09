#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <string>
#include <vector>

namespace editor
{

class UngroupShapesCommand : public ICommand
{
public:
	UngroupShapesCommand(Picture& picture, std::string groupId);
	void Execute() override;
	void Unexecute() override;
	std::string GetName() const override;
	std::vector<std::string> GetShapeIds() const override;

private:
	Picture& m_picture;
	std::string m_groupId;
	std::vector<std::string> m_ids;
	LifeStyle m_lifetime;
	int m_index = 0;
};

} // namespace editor
