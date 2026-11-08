#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <memory>
#include <string>
#include <vector>

namespace editor
{

class AddShapeCommand : public ICommand
{
public:
	AddShapeCommand(Picture& picture, std::unique_ptr<Shape> shape);
	void Execute() override;
	void Unexecute() override;
	std::string GetName() const override;
	std::vector<std::string> GetShapeIds() const override;

private:
	Picture& m_picture;
	std::string m_id;
	std::unique_ptr<Shape> m_shape;
};

} // namespace editor
