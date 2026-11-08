#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <string>
#include <vector>

namespace editor
{

class ChangeLifetimeCommand : public ICommand
{
public:
	ChangeLifetimeCommand(Picture& picture, std::string id, LifeStyle oldLifetime, LifeStyle newLifetime);
	void Execute() override;
	void Unexecute() override;
	std::string GetName() const override;
	std::vector<std::string> GetShapeIds() const override;

private:
	Picture& m_picture;
	std::string m_id;
	LifeStyle m_oldLifetime;
	LifeStyle m_newLifetime;
};

} // namespace editor
