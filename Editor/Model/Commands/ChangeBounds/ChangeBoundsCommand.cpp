#include "ChangeBoundsCommand.h"

#include <utility>

namespace editor
{

ChangeBoundsCommand::ChangeBoundsCommand(
	Picture& picture, std::string id, Rect oldBounds, Rect newBounds)
	: m_picture(picture)
	, m_id(std::move(id))
	, m_oldBounds(oldBounds)
	, m_newBounds(newBounds)
{
}

void ChangeBoundsCommand::Execute() { m_picture.GetShape(m_id).SetBounds(m_newBounds); }

void ChangeBoundsCommand::Unexecute() { m_picture.GetShape(m_id).SetBounds(m_oldBounds); }

std::string ChangeBoundsCommand::GetName() const { return "Change Bounds"; }

std::vector<std::string> ChangeBoundsCommand::GetShapeIds() const { return { m_id }; }

} // namespace editor
