#pragma once

#include "Model/Document.h"

#include <memory>

namespace editor
{

class IWindowService
{
public:
	virtual ~IWindowService() = default;
	virtual void OpenWindow(std::shared_ptr<Document> document) = 0;
};

} // namespace editor
