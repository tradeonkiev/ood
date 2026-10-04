#pragma once

#include <cstdint>
#include <iomanip>
#include <istream>
#include <sstream>
#include <string>

namespace gfx {

    struct Color {
        std::uint8_t red = 0;
        std::uint8_t green = 0;
        std::uint8_t blue = 0;

        std::string ToHexString() const {
            std::ostringstream output;
            output << '#' << std::hex << std::setfill('0') << std::setw(2) << +red << std::setw(2) << +green
                   << std::setw(2) << +blue;
            return output.str();
        }

        bool operator==(const Color &) const = default;

        friend std::istream &operator>>(std::istream &input, Color &color) {
            char hash = 0;
            unsigned value = 0;
            input >> hash >> std::hex >> value >> std::dec;

            color.red = static_cast<std::uint8_t>((value >> 16) & 0xff);
            color.green = static_cast<std::uint8_t>((value >> 8) & 0xff);
            color.blue = static_cast<std::uint8_t>(value & 0xff);
            return input;
        }
    };

} // namespace gfx
