#include <iostream>
#include "SFML/Graphics.hpp"
#include "Quadtree.h"

// Estructura para representar un punto en la simulación
struct SimPoint {
    double x, y;
    SimPoint(double _x, double _y) : x(_x), y(_y) {}
};

// Función para convertir un punto del QuadTree a un punto de la simulación
SimPoint quadTreePointToSimPoint(const Point& qtPoint) {
    return SimPoint(qtPoint.x, qtPoint.y);
}

// Función para dibujar un rectángulo en la ventana de SFML
void drawRectangle(sf::RenderWindow& window, const Rectangle& boundary) {
    sf::RectangleShape rectangle(sf::Vector2f(boundary.w, boundary.h));
    rectangle.setPosition(boundary.x - boundary.w / 2, boundary.y - boundary.h / 2);
    rectangle.setOutlineColor(sf::Color::Black);
    rectangle.setOutlineThickness(1);
    rectangle.setFillColor(sf::Color::Transparent);
    window.draw(rectangle);
}

int main() {
    // Configuración de la ventana SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "QuadTree Simulation");
    window.setFramerateLimit(60);

    // Creación del QuadTree y puntos de ejemplo
    Rectangle boundary(400, 300, 800, 600);
    // Usando la capacidad por defecto
    //QuadTree quadTree(boundary);

    // Especificando una capacidad diferente
    int customCapacity = 12; // Puedes cambiar este valor según tus necesidades
    QuadTree quadTree(boundary, customCapacity);
    

    Point point1(200, 300);
    Point point2(500, 400);
    Point point3(700, 200);

    quadTree.insert(point1);
    quadTree.insert(point2);
    quadTree.insert(point3);

    // Punto de búsqueda
    Point searchPoint(100, 300);

    // Variable para saber si el botón del ratón está presionado
    bool mouseHeldDown = false;

    // Bucle principal de la simulación
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseHeldDown = true;
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseHeldDown = false;
                }
            }
        }

        // Si el botón del ratón está presionado, inserta puntos en el QuadTree
        if (mouseHeldDown) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            Point newPoint(mousePos.x, mousePos.y);
            quadTree.insert(newPoint);
        }

        // Lógica de la simulación

        // Realiza una búsqueda en el QuadTree alrededor del punto de búsqueda
        std::vector<Point> searchResult = quadTree.closest(searchPoint, 5, 200);

        // Renderizado
        window.clear(sf::Color::White); // Limpia la ventana con el color de fondo

        // Dibuja el QuadTree (líneas de las subdivisiones)
        std::function<void(QuadTree*)> drawQuadTree;
        drawQuadTree = [&window, &drawQuadTree](QuadTree* qt) {
            drawRectangle(window, qt->getBoundary()); // Dibuja el rectángulo del nodo actual
            if (qt->isDivided()) {
                for (auto child : qt->getChildren()) {
                    drawQuadTree(child); // Recursivamente dibuja los hijos
                }
            }
        };

        drawQuadTree(&quadTree); // Comienza a dibujar desde la raíz

        // Dibuja círculos verdes alrededor de los puntos encontrados
        for (const auto& p : searchResult) {
            SimPoint simPoint = quadTreePointToSimPoint(p);
            sf::CircleShape circle(8);
            circle.setPosition(simPoint.x - 8, simPoint.y - 8);
            circle.setFillColor(sf::Color::Green);
            window.draw(circle);
        }

        // Dibuja puntos rojos para todos los puntos en el QuadTree
        quadTree.for_each([&window](const Point& p) {
            SimPoint simPoint = quadTreePointToSimPoint(p);
            sf::CircleShape circle(5);
            circle.setPosition(simPoint.x - 5, simPoint.y - 5);
            circle.setFillColor(sf::Color::Red);
            window.draw(circle);
        });

        // Dibujar punto de búsqueda (si es necesario)
        // ...

        window.display(); // Actualiza la ventana con todo lo que se ha dibujado
    }

    return 0;
}
