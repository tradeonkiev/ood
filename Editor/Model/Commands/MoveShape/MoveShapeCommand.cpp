#include "MoveShapeCommand.h"

#include <utility>

namespace editor
{

MoveShapeCommand::MoveShapeCommand(Picture& picture, std::string id, int newIndex)
	: m_picture(picture)
	, m_id(std::move(id))
	, m_newIndex(newIndex)
{
}

void MoveShapeCommand::Execute()
{
	m_oldIndex = m_picture.GetShapeIndex(m_id);
	m_picture.MoveShape(m_id, m_newIndex);
}

void MoveShapeCommand::Unexecute() { m_picture.MoveShape(m_id, m_oldIndex); }

std::string MoveShapeCommand::GetName() const { return "Move Shape"; }

std::vector<std::string> MoveShapeCommand::GetShapeIds() const { return { m_id }; }

} // namespace editor
