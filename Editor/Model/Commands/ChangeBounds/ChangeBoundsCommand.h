#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <string>
#include <vector>

namespace editor
{

class ChangeBoundsCommand : public ICommand
{
public:
	ChangeBoundsCommand(Picture& picture, std::string id, Rect oldBounds, Rect newBounds);
	void Execute() override;
	void Unexecute() override;
	std::string GetName() const override;
	std::vector<std::string> GetShapeIds() const override;

private:
	Picture& m_picture;
	std::string m_id;
	Rect m_oldBounds;
	Rect m_newBounds;
};
} // namespace editor
