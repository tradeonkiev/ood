#pragma once

#include <ostream>

namespace shapes {

    struct Point {
        double x = 0;
        double y = 0;

        bool operator==(const Point &anotherPoint) const { return anotherPoint.x == x && anotherPoint.y == y; };

        friend std::ostream &operator<<(std::ostream &os, const Point anotherPoint) {
            os << "(" << anotherPoint.x << ", " << anotherPoint.y << ")";
            return os;
        }
    };

    struct Rect {
        double left = 0;
        double top = 0;
        double width = 0;
        double height = 0;

        double Right() const { return left + width; }
        double Bottom() const { return top + height; }

        bool operator==(const Rect &anotherRect) const {
            return anotherRect.left == left && anotherRect.top == top && anotherRect.width == width &&
                   anotherRect.height == height;
        };

        friend std::ostream &operator<<(std::ostream &os, const Rect anotherRect) {
            os << "(left: " << anotherRect.left << ", top: " << anotherRect.top << ", width: " << anotherRect.width
               << ", height: " << anotherRect.height  << ")";
            return os;
        }
    };

} // namespace shapes
