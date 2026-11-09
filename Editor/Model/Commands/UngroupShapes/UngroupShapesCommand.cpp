#include "UngroupShapesCommand.h"

#include "Model/Shapes/GroupShape.h"

#include <utility>

namespace editor
{

UngroupShapesCommand::UngroupShapesCommand(Picture& picture, std::string groupId)
	: m_picture(picture)
	, m_groupId(std::move(groupId))
{
	const GroupShape* group = m_picture.GetShape(m_groupId).GetGroup();
	for (int i = 0; i < group->GetShapeCount(); i++)
	{
		m_ids.push_back(group->GetShapeAt(i).GetId());
	}
}

void UngroupShapesCommand::Execute()
{
	m_index = m_picture.GetShapeIndex(m_groupId);
	auto group = m_picture.DeleteShape(m_groupId);
	m_lifetime = group->GetLifetime();
	auto shapes = group->GetGroup()->TakeShapes();
	for (size_t i = 0; i < shapes.size(); i++)
	{
		m_picture.AddShape(std::move(shapes[i]), m_index + static_cast<int>(i));
	}
}

void UngroupShapesCommand::Unexecute()
{
	std::vector<std::unique_ptr<Shape>> shapes;
	for (const auto& id : m_ids)
	{
		shapes.push_back(m_picture.DeleteShape(id));
	}
	auto group = std::make_unique<GroupShape>(m_groupId, std::move(shapes));
	group->SetLifetime(m_lifetime);
	m_picture.AddShape(std::move(group), m_index);
}

std::string UngroupShapesCommand::GetName() const { return "Ungroup Shapes"; }

std::vector<std::string> UngroupShapesCommand::GetShapeIds() const
{
	std::vector<std::string> ids = m_ids;
	ids.push_back(m_groupId);
	return ids;
}

} // namespace editor
