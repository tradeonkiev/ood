#pragma once

#include "Canvas/Color.h"

#include <array>
#include <optional>

namespace editor
{

class ColorDialog
{
public:
	explicit ColorDialog(void* owner);

	std::optional<Color> AskColor(Color initial);

private:
	void* m_owner;
	std::array<unsigned long, 16> m_customColors{};
};

} // namespace editor
