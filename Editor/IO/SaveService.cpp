#include "SaveService.h"

namespace editor
{

void SaveService::Save(const Picture& picture, std::ostream& out)
{
	for (int i = 0; i < picture.GetShapeCount(); i++)
	{
		const Shape& shape = picture.GetShapeAt(i);
		const Rect bounds = shape.GetBounds();
		out << shape.GetType() << ' ' << shape.GetColor().ToHexString() << ' ' << bounds.left << ' ' << bounds.top
			<< ' ' << bounds.width << ' ' << bounds.height << '\n';
	}
}

} // namespace editor
