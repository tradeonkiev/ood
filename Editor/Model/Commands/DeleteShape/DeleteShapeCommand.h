#pragma once

#include "Model/Commands/ICommand.h"
#include "Model/Picture.h"

#include <cstddef>
#include <memory>
#include <string>

namespace editor
{

class DeleteShapeCommand : public ICommand
{
public:
	DeleteShapeCommand(Picture& picture, std::string id);
	void Execute() override;
	void Unexecute() override;

private:
	Picture& m_picture;
	std::string m_id;
	int m_index = 0;
	std::unique_ptr<Shape> m_shape;
};

} // namespace editor
