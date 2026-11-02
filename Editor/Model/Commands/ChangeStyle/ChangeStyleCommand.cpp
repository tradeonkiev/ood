#include "ChangeStyleCommand.h"

#include <utility>

namespace editor
{

ChangeStyleCommand::ChangeStyleCommand(Picture& picture, std::string id, ShapeStyle oldStyle, ShapeStyle newStyle)
	: m_picture(picture)
	, m_id(std::move(id))
	, m_oldStyle(oldStyle)
	, m_newStyle(newStyle)
{
}

void ChangeStyleCommand::Execute() { m_picture.GetShape(m_id).SetStyle(m_newStyle); }

void ChangeStyleCommand::Unexecute() { m_picture.GetShape(m_id).SetStyle(m_oldStyle); }

std::string ChangeStyleCommand::GetName() const { return "Change Style"; }
} // namespace editor
