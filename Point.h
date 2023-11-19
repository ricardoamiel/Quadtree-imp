#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>

class Point {
public:
    double x, y;
    // Puedes agregar más campos de datos según sea necesario

    Point(double _x, double _y) : x(_x), y(_y) {}
    
    bool operator==(const Point& other) const
    {
      return x == other.x && y == other.y;
    }
    
    bool operator!=(const Point& other) const
    {
      return !(*this == other);
    }
    
    double sqDistanceFrom(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return dx * dx + dy * dy;
    }
    
};

class MovingPoint : public Point {
public:
    double vx, vy; // Velocidades en el eje x e y

    MovingPoint(double _x, double _y, double _vx, double _vy) 
    : Point(_x, _y), vx(_vx), vy(_vy) {}

    // Actualiza la posición basada en la velocidad
    void update(double deltaTime) {
        x += vx * deltaTime;
        y += vy * deltaTime;
    }

    // Rebotar en horizontal
    void bounceX() {
        vx = -vx;
    }

    // Rebotar en vertical
    void bounceY() {
        vy = -vy;
    }
};
