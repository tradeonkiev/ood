#pragma once

#include "Model/Commands/ICommand.h"
#include "Picture/Picture.h"

#include <cstddef>
#include <memory>
#include <string>

namespace ebator
{

class DeleteShapeCommand : public ICommand
{
public:
	DeleteShapeCommand(shapes::Picture& picture, std::string id);
	void Execute() override;
	void Unexecute() override;

private:
	shapes::Picture& m_picture;
	std::string m_id;
	int m_index = 0;
	std::unique_ptr<shapes::Shape> m_shape;
};

} // namespace ebator
