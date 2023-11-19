#include <iostream>
#include <SFML/Graphics.hpp>
#include "Quadtree.h"

// Función para generar una velocidad inicial aleatoria
double randomVelocity() {
    return static_cast<double>(rand() % 100 - 50); // Velocidad aleatoria entre -50 y 50
}

int main() {
    // Configuración de la ventana SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "QuadTree Simulation");
    window.setFramerateLimit(60);

    // Creación del QuadTree
    Rectangle boundary(400, 300, 800, 600);
    QuadTree quadTree(boundary, 16);

    // Lista para almacenar y actualizar tus puntos
    std::vector<MovingPoint> points;

    // Añadir algunos puntos de prueba
    points.emplace_back(200, 300, 50, -50);
    points.emplace_back(500, 400, -50, 50);
    points.emplace_back(700, 200, -50, -50);
    points.emplace_back(400, 400, 50, 50);
    // ... Añadir más puntos según sea necesario ...

    sf::Clock clock; // Reloj para manejar el tiempo entre frames

    sf::Font font;
    if (!font.loadFromFile("fonts/sofia.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return -1;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);

    bool mouseHeldDown = false; // Flag para controlar si el botón del ratón está presionado

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Evento para cuando se presiona el botón del ratón
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseHeldDown = true;
                }
            }
            // Evento para cuando se suelta el botón del ratón
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    mouseHeldDown = false;
                }
            }
        }

        // Crear y mover puntos si el mouse está presionado
        if (mouseHeldDown) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            MovingPoint newPoint(mousePos.x, mousePos.y, randomVelocity(), randomVelocity());
            points.push_back(newPoint);
            // No necesitas insertar el nuevo punto aquí si ya lo estás haciendo en la actualización de abajo
        }

        sf::Time elapsed = clock.restart(); // Tiempo transcurrido desde el último frame
        double deltaTime = elapsed.asSeconds();

        // Actualización de puntos
        quadTree.clear(); // Limpiar el QuadTree antes de volver a insertar los puntos
        for (auto& point : points) {
            point.update(deltaTime); // Actualiza la posición basada en la velocidad

            // Rebotar contra los bordes del área de simulación
            if (point.x < 0 || point.x > window.getSize().x) point.bounceX();
            if (point.y < 0 || point.y > window.getSize().y) point.bounceY();

            // ... Actualizar el QuadTree ...
            // Inserta el punto en el QuadTree
            quadTree.insert(point);
        }

        // Detección de colisiones y respuesta
        // ... Lógica para detectar y responder a las colisiones ...
        // Suponemos que los puntos no se superponen inicialmente
        for (size_t i = 0; i < points.size(); ++i) {
            for (size_t j = i + 1; j < points.size(); ++j) {
                double dx = points[j].x - points[i].x;
                double dy = points[j].y - points[i].y;
                double distanceSquared = dx * dx + dy * dy;
                if (distanceSquared < 9) {  // Suponiendo que los puntos tienen un radio de 3
                    points[i].bounceX();
                    points[i].bounceY();
                    points[j].bounceX();
                    points[j].bounceY();
                }
            }
        }

        // Renderización
        window.clear(sf::Color::White);

        // Dibuja puntos
        for (const auto& point : points) {
            sf::CircleShape shape(3); // Tamaño de los puntos
            shape.setPosition(static_cast<float>(point.x - 3), static_cast<float>(point.y - 3)); // Centrar el punto
            shape.setFillColor(sf::Color::Red);
            window.draw(shape);
        }

        // ... Dibujo de subdivisiones del QuadTree y estadísticas ...
        std::function<void(sf::RenderWindow&, QuadTree*)> drawQuadTree;
        drawQuadTree = [&](sf::RenderWindow &window, QuadTree* node) {
            if (node == nullptr) return;

            // Dibuja el borde del nodo actual
            Rectangle boundary = node->getBoundary();
            sf::RectangleShape rectangle(sf::Vector2f(boundary.w, boundary.h));
            rectangle.setPosition(boundary.x - boundary.w / 2, boundary.y - boundary.h / 2);
            rectangle.setOutlineColor(sf::Color::Black);
            rectangle.setOutlineThickness(1);
            rectangle.setFillColor(sf::Color::Transparent);
            window.draw(rectangle);

            // Dibuja las subdivisiones de los nodos hijos
            if (node->isDivided()) {
                drawQuadTree(window, node-> getNortheast());
                drawQuadTree(window, node->getNorthwest());
                drawQuadTree(window, node->getSoutheast());
                drawQuadTree(window, node->getSouthwest());
            }
        };

        // Dibuja puntos
        for (const auto& point : points) {
            sf::CircleShape shape(3); // Tamaño de los puntos
            shape.setPosition(static_cast<float>(point.x - 3), static_cast<float>(point.y - 3)); // Centrar el punto
            shape.setFillColor(sf::Color::Red);
            window.draw(shape);
        }

        // Dibujar subdivisiones
        drawQuadTree(window, &quadTree);
        

        // Actualizar y dibujar estadísticas
        text.setString("Objects: " + std::to_string(points.size()));
        window.draw(text);

        window.display();
    }

    return 0;
}
