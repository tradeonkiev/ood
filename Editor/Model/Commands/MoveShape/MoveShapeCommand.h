#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <string>
#include <vector>

namespace editor
{

class MoveShapeCommand : public ICommand
{
public:
	MoveShapeCommand(Picture& picture, std::string id, int newIndex);
	void Execute() override;
	void Unexecute() override;
	std::string GetName() const override;
	std::vector<std::string> GetShapeIds() const override;

private:
	Picture& m_picture;
	std::string m_id;
	int m_oldIndex = 0;
	int m_newIndex = 0;
};

} // namespace editor
