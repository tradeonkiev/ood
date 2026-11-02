#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <string>

namespace editor
{

class ChangeStyleCommand : public ICommand
{
public:
	ChangeStyleCommand(Picture& picture, std::string id, ShapeStyle oldStyle, ShapeStyle newStyle);
	void Execute() override;
	void Unexecute() override;
	std::string GetName() const override;

private:
	Picture& m_picture;
	std::string m_id;
	ShapeStyle m_oldStyle;
	ShapeStyle m_newStyle;
};

} // namespace editor
