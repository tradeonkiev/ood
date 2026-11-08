#include "ChangeLifetimeCommand.h"

#include <utility>

namespace editor
{

ChangeLifetimeCommand::ChangeLifetimeCommand(
	Picture& picture, std::string id, LifeStyle oldLifetime, LifeStyle newLifetime)
	: m_picture(picture)
	, m_id(std::move(id))
	, m_oldLifetime(oldLifetime)
	, m_newLifetime(newLifetime)
{
}

void ChangeLifetimeCommand::Execute() { m_picture.GetShape(m_id).SetLifetime(m_newLifetime); }

void ChangeLifetimeCommand::Unexecute() { m_picture.GetShape(m_id).SetLifetime(m_oldLifetime); }

std::string ChangeLifetimeCommand::GetName() const { return "Change LifeStyle"; }

std::vector<std::string> ChangeLifetimeCommand::GetShapeIds() const { return { m_id }; }

} // namespace editor
