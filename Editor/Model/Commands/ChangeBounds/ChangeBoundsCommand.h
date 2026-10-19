#pragma once

#include "Model/Commands/ICommand.h"
#include "Picture/Picture.h"

#include <string>

namespace ebator
{

class ChangeBoundsCommand : public ICommand
{
public:
	ChangeBoundsCommand(shapes::Picture& picture, std::string id, shapes::Rect oldBounds, shapes::Rect newBounds);
	void Execute() override;
	void Unexecute() override;

private:
	shapes::Picture& m_picture;
	std::string m_id;
	shapes::Rect m_oldBounds;
	shapes::Rect m_newBounds;
};
} // namespace ebator
