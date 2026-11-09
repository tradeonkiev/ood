#include "Document.h"

#include "Model/Shapes/GroupShape.h"

#include <format>
#include <utility>

namespace editor
{

namespace
{

int CountShapes(const Shape& shape)
{
	const GroupShape* group = shape.GetGroup();
	if (!group)
	{
		return 1;
	}

	int count = 1;
	for (int i = 0; i < group->GetShapeCount(); i++)
	{
		count += CountShapes(group->GetShapeAt(i));
	}
	return count;
}

} // namespace

Document::Document()
{
	static int untitledCount = 0;
	m_untitledName = std::format("doc#{}", untitledCount++);
}

Document::Document(std::vector<std::unique_ptr<Shape>> shapes, std::filesystem::path path)
	: m_filePath(std::move(path))
{
	for (auto& shape : shapes)
	{
		m_nextShapeId += CountShapes(*shape);
		m_picture.AddShape(std::move(shape));
	}
}

Picture& Document::GetPicture() { return m_picture; }

History& Document::GetHistory() { return m_history; }

std::string Document::GenerateShapeId() { return std::format("shape{}", m_nextShapeId++); }

const std::optional<std::filesystem::path>& Document::GetFilePath() const { return m_filePath; }

void Document::SetFilePath(std::filesystem::path path) { m_filePath = std::move(path); }

std::string Document::GetTitle() const
{
	if (!m_filePath)
	{
		return m_untitledName;
	}
	const std::u8string name = m_filePath->filename().u8string();
	return { name.begin(), name.end() };
}

} // namespace editor
