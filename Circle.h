#include "Rectangle.h"

class Circle {
public:
    double x, y, r;
    double rSquared;

    Circle(double _x, double _y, double _r) : x(_x), y(_y), r(_r) {
        rSquared = r * r;
    }

    bool contains(const Point& point) const {
        double d = std::pow((point.x - x), 2) + std::pow((point.y - y), 2);
        return d <= rSquared;
    }

    bool intersects(const Rectangle& range) const {
        double xDist = std::abs(range.x - x);
        double yDist = std::abs(range.y - y);
        double w = range.w / 2;
        double h = range.h / 2;
        double edges = std::pow((xDist - w), 2) + std::pow((yDist - h), 2);

        if (xDist > (r + w) || yDist > (r + h)) {
            return false;
        }
        if (xDist <= w || yDist <= h) {
            return true;
        }
        return edges <= rSquared;
    }
};