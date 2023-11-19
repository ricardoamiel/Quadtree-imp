#include <iostream>
#include "Quadtree.h"

int main() {
    // Ejemplo de uso del QuadTree en C++
    // Puedes adaptar y extender este código según tus necesidades

    Rectangle boundary(0, 0, 100, 100);
    QuadTree quadTree(boundary, 4);

    // Insertar puntos en el QuadTree
    Point point1(20, 30);
    Point point2(50, 60);
    Point point3(80, 90);

    quadTree.insert(point1);
    quadTree.insert(point2);
    quadTree.insert(point3);

    // Consultar puntos en un rango
    Rectangle queryRange(10, 20, 30, 40);
    std::vector<Point> pointsInQuery = quadTree.query(queryRange);

    // Mostrar puntos en el rango consultado
    std::cout << "Puntos en el rango consultado:" << std::endl;
    for (const auto& point : pointsInQuery) {
        std::cout << "(" << point.x << ", " << point.y << ")" << std::endl;
    }

    return 0;
}
