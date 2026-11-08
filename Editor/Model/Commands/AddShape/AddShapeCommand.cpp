#include "AddShapeCommand.h"

#include <utility>

namespace editor
{

AddShapeCommand::AddShapeCommand(Picture& picture, std::unique_ptr<Shape> shape)
	: m_picture(picture)
	, m_id(shape->GetId())
	, m_shape(std::move(shape))
{
}

void AddShapeCommand::Execute() { m_picture.AddShape(std::move(m_shape)); }

void AddShapeCommand::Unexecute() { m_shape = m_picture.DeleteShape(m_id); }

std::string AddShapeCommand::GetName() const { return "Add Shape"; }

std::vector<std::string> AddShapeCommand::GetShapeIds() const { return { m_id }; }

} // namespace editor
