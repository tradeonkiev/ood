#include "SaveService.h"

#include "Model/Shapes/GroupShape.h"

namespace editor
{

void SaveService::Save(const Picture& picture, std::ostream& out)
{
	for (int i = 0; i < picture.GetShapeCount(); i++)
	{
		SaveShape(picture.GetShapeAt(i), out);
	}
}

void SaveService::SaveShape(const Shape& shape, std::ostream& out)
{
	const LifeStyle lifetime = shape.GetLifetime();
	if (const GroupShape* group = shape.GetGroup())
	{
		out << shape.GetType() << ' ' << group->GetShapeCount() << ' ' << lifetime.start << ' ' << lifetime.end << '\n';
		for (int i = 0; i < group->GetShapeCount(); i++)
		{
			SaveShape(group->GetShapeAt(i), out);
		}
		return;
	}

	const Rect bounds = shape.GetBounds();
	const ShapeStyle style = shape.GetStyle();
	out << shape.GetType() << ' ' << style.fillColor.ToHexString() << ' ' << style.strokeColor.ToHexString() << ' '
		<< bounds.left << ' ' << bounds.top << ' ' << bounds.width << ' ' << bounds.height << ' ' << lifetime.start
		<< ' ' << lifetime.end << '\n';
}

} // namespace editor
