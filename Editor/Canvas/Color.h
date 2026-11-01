#pragma once

#include <cstdint>
#include <format>
#include <string>

namespace editor
{

struct Color
{
	std::uint8_t red = 0;
	std::uint8_t green = 0;
	std::uint8_t blue = 0;

	std::string ToHexString() const { return std::format("#{:02x}{:02x}{:02x}", red, green, blue); }

	bool operator==(const Color&) const = default;
};

} // namespace editor
