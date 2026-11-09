#include "GroupShapesCommand.h"

#include "Model/Shapes/GroupShape.h"

#include <utility>

namespace editor
{

GroupShapesCommand::GroupShapesCommand(Picture& picture, std::string groupId, std::vector<std::string> ids)
	: m_picture(picture)
	, m_groupId(std::move(groupId))
	, m_ids(std::move(ids))
{
}

void GroupShapesCommand::Execute()
{
	m_indices.clear();
	for (const auto& id : m_ids)
	{
		m_indices.push_back(m_picture.GetShapeIndex(id));
	}

	std::vector<std::unique_ptr<Shape>> shapes;
	for (const auto& id : m_ids)
	{
		shapes.push_back(m_picture.DeleteShape(id));
	}
	m_picture.AddShape(std::make_unique<GroupShape>(m_groupId, std::move(shapes)));
}

void GroupShapesCommand::Unexecute()
{
	auto group = m_picture.DeleteShape(m_groupId);
	auto shapes = group->GetGroup()->TakeShapes();
	for (size_t i = 0; i < shapes.size(); i++)
	{
		m_picture.AddShape(std::move(shapes[i]), m_indices[i]);
	}
}

std::string GroupShapesCommand::GetName() const { return "Group Shapes"; }

std::vector<std::string> GroupShapesCommand::GetShapeIds() const
{
	std::vector<std::string> ids = m_ids;
	ids.push_back(m_groupId);
	return ids;
}

} // namespace editor
