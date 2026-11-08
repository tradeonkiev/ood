#include "SaveService.h"

namespace editor
{

void SaveService::Save(const Picture& picture, std::ostream& out)
{
	for (int i = 0; i < picture.GetShapeCount(); i++)
	{
		const Shape& shape = picture.GetShapeAt(i);
		const Rect bounds = shape.GetBounds();
		const ShapeStyle style = shape.GetStyle();
		const LifeStyle lifetime = shape.GetLifetime();
		out << shape.GetType() << ' ' << style.fillColor.ToHexString() << ' ' << style.strokeColor.ToHexString()
			<< ' ' << bounds.left << ' ' << bounds.top << ' ' << bounds.width << ' ' << bounds.height << ' '
			<< lifetime.start << ' ' << lifetime.end << '\n';
	}
}

} // namespace editor
