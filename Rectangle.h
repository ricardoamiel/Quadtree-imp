#include "Point.h"

class Rectangle {
public:
    double x, y, w, h;
    double left, right, top, bottom;

    Rectangle(double _x, double _y, double _w, double _h) : x(_x), y(_y), w(_w), h(_h) {
        left = x - w / 2;
        right = x + w / 2;
        top = y - h / 2;
        bottom = y + h / 2;
    }

    bool contains(const Point& point) const {
        return (left <= point.x && point.x <= right && top <= point.y && point.y <= bottom);
    }

    bool intersects(const Rectangle& range) const {
        return !(right < range.left || range.right < left || bottom < range.top || range.bottom < top);
    }

    Rectangle subdivide(const std::string& quadrant) const {
        if (quadrant == "ne") {
            return Rectangle(x + w / 4, y - h / 4, w / 2, h / 2);
        } else if (quadrant == "nw") {
            return Rectangle(x - w / 4, y - h / 4, w / 2, h / 2);
        } else if (quadrant == "se") {
            return Rectangle(x + w / 4, y + h / 4, w / 2, h / 2);
        } else if (quadrant == "sw") {
            return Rectangle(x - w / 4, y + h / 4, w / 2, h / 2);
        } else {
            // Manejo de error o lanzamiento de excepción
            throw std::invalid_argument("Invalid quadrant");
            return Rectangle(0, 0, 0, 0);
        }
    }

    double xDistanceFrom(const Point& point) const {
        if (left <= point.x && point.x <= right) {
            return 0;
        }
        return std::min(std::abs(point.x - left), std::abs(point.x - right));
    }

    double yDistanceFrom(const Point& point) const {
        if (top <= point.y && point.y <= bottom) {
            return 0;
        }
        return std::min(std::abs(point.y - top), std::abs(point.y - bottom));
    }

    double sqDistanceFrom(const Point& point) const {
        double dx = xDistanceFrom(point);
        double dy = yDistanceFrom(point);
        return dx * dx + dy * dy;
    }

    double distanceFrom(const Point& point) const {
        return std::sqrt(sqDistanceFrom(point));
    }
};