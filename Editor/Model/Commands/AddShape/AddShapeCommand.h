#pragma once

#include "Model/Commands/ICommand.h"
#include "Picture/Picture.h"

#include <memory>
#include <string>

namespace ebator
{

class AddShapeCommand : public ICommand
{
public:
	AddShapeCommand(shapes::Picture& picture, std::unique_ptr<shapes::Shape> shape);
	void Execute() override;
	void Unexecute() override;

private:
	shapes::Picture& m_picture;
	std::string m_id;
	std::unique_ptr<shapes::Shape> m_shape;
};

} // namespace ebator
