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
    int customCapacity = 4; // Puedes cambiar este valor según tus necesidades
    QuadTree quadTree(boundary, customCapacity);
    
    // Vector para almacenar los puntos de ejemplo
    std::vector<Point> points;

    Point point1(200, 300);
    Point point2(500, 400);
    Point point3(700, 200);

    quadTree.insert(point1);
    quadTree.insert(point2);
    quadTree.insert(point3);

    points.emplace_back(point1);
    points.emplace_back(point2);
    points.emplace_back(point3);

    // Punto de búsqueda
    Point searchPoint(100, 300);

    sf::Font font;
    if (!font.loadFromFile("fonts/sofia.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);

    // Bucle principal de la simulación
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                Point clickedPoint(mousePos.x, mousePos.y);
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Insertar un nuevo punto en el QuadTree con un solo clic
                    quadTree.insert(clickedPoint);
                    points.emplace_back(clickedPoint);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    // Crear un rango alrededor del punto donde se hizo clic
                    Rectangle range(mousePos.x, mousePos.y, 15, 15); // El tamaño del rango puede ser ajustado
                    quadTree.delete_in_range(range);

                    // Después de la eliminación, intenta unir si es necesario
                    while (quadTree.tryMerge()) {
                        // Continúa intentando unir mientras sea posible


                        // Si se realizó una unión, reconstruye el vector de puntos
                        points.clear();
                        quadTree.for_each([&points](const Point& p) {
                            points.push_back(p);
                        });
                    }
                }        
            }
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

        // Actualizar y dibujar estadísticas de colisiones
        text.setString("Objeto: " + std::to_string(points.size()));
        window.draw(text);

        // ...

        window.display(); // Actualiza la ventana con todo lo que se ha dibujado
    }

    return 0;
}