#pragma once

#include "Strategies/IShapeGeometry.h"

class LineStrategy : public shapes::IShapeGeometry
{
public:
	LineStrategy(shapes::Point start, shapes::Point end);
	void Draw(gfx::ICanvas& canvas, gfx::Color color) const override;
	void Move(double dx, double dy) override;
	bool ContainsPoint(const shapes::Point& point) const override;

	void SetBounds(const shapes::Rect& bounds) override;
	shapes::Rect GetBounds() const override;
	std::string GetType() const override;
	std::string GetParameters() const override;

private:
	shapes::Point m_start;
	shapes::Point m_end;
};
