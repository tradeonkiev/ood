#include "ChangeBoundsCommand.h"

#include <utility>

namespace ebator
{

ChangeBoundsCommand::ChangeBoundsCommand(
	shapes::Picture& picture, std::string id, shapes::Rect oldBounds, shapes::Rect newBounds)
	: m_picture(picture)
	, m_id(std::move(id))
	, m_oldBounds(oldBounds)
	, m_newBounds(newBounds)
{
}

void ChangeBoundsCommand::Execute() { m_picture.GetShape(m_id).SetBounds(m_newBounds); }

void ChangeBoundsCommand::Unexecute() { m_picture.GetShape(m_id).SetBounds(m_oldBounds); }

} // namespace ebator
