#include "DeleteShapeCommand.h"

#include <utility>

namespace ebator
{

DeleteShapeCommand::DeleteShapeCommand(shapes::Picture& picture, std::string id)
	: m_picture(picture)
	, m_id(std::move(id))
{
}

void DeleteShapeCommand::Execute()
{
	m_index = m_picture.GetShapeIndex(m_id);
	m_shape = m_picture.DeleteShape(m_id);
}

void DeleteShapeCommand::Unexecute() { m_picture.AddShape(std::move(m_shape), m_index); }

} // namespace ebator
