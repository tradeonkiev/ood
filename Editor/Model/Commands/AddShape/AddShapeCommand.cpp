#include "AddShapeCommand.h"

#include <utility>

namespace ebator
{

AddShapeCommand::AddShapeCommand(shapes::Picture& picture, std::unique_ptr<shapes::Shape> shape)
	: m_picture(picture)
	, m_id(shape->GetId())
	, m_shape(std::move(shape))
{
}

void AddShapeCommand::Execute() { m_picture.AddShape(std::move(m_shape)); }

void AddShapeCommand::Unexecute() { m_shape = m_picture.DeleteShape(m_id); }

} // namespace ebator
