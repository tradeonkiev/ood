#pragma once

#include "Shape/Shape.h"

#include "../Observer/IObserver.h"
#include "../Observer/Observable.h"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace shapes
{
class Picture : public observer::IObserver<Shape>
	, public observer::Observable<Picture>
{
public:
	void AddShape(std::unique_ptr<Shape> shape);
	std::unique_ptr<Shape> DeleteShape(const std::string& id);
	void Move(double dx, double dy);
	void Draw(gfx::ICanvas& canvas) const;
	Shape& GetShape(const std::string& id);
	const Shape& GetShape(const std::string& id) const;
	const Shape& GetShapeAt(std::size_t index) const;
	std::size_t GetShapeCount() const noexcept;

	void Update() override;

private:
	Shape* FindShape(const std::string& id) noexcept;
	const Shape* FindShape(const std::string& id) const noexcept;
	std::vector<std::unique_ptr<Shape>> m_shapes;
	std::unordered_map<std::string, Shape*> m_shapesById;
	std::unordered_map<std::string, observer::Subscription> m_shapeSubscriptions;
};

} // namespace shapes
