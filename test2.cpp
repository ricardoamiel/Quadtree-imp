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

    int collisionCountThisFrame = 0; // Este contador se reinicia cada frame

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

        // 1. Actualizar la posición de los puntos
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

        // 2. Restablecer el flag de colisión
        for (auto& point : points) {
            point.resetCollisionFlag(); // Resetea el flag de colisión antes de la detección
        }

        // Reinicia el contador de colisiones cada frame
        //collisionCountThisFrame = 0;

        // 3. Detección de colisiones y respuesta
        for (size_t i = 0; i < points.size(); ++i) {
            for (size_t j = i + 1; j < points.size(); ++j) {
                if (!points[i].collidedThisFrame && !points[j].collidedThisFrame) {
                    double dx = points[j].x - points[i].x;
                    double dy = points[j].y - points[i].y;
                    double distanceSquared = dx * dx + dy * dy;
                    // Después de detectar una colisión
                    if (distanceSquared < 9) {
                        // Calcular el vector normal de la colisión
                        double nx = dx / sqrt(distanceSquared);
                        double ny = dy / sqrt(distanceSquared);

                        // Calcular la velocidad relativa en la dirección normal
                        double relVel = points[i].vx * nx + points[i].vy * ny
                                    - points[j].vx * nx - points[j].vy * ny;

                        // Solo rebota si los objetos se están moviendo uno hacia el otro
                        if (relVel > 0) continue;

                        // Aplicar una fuerza de rebote (esto es un ejemplo simple y podría no ser físicamente preciso)
                        double bounce = 2 * relVel;
                        double dumping = 1; // Factor de amortiguacion para reducir la velocidad después de la colisión
                        points[i].vx -= bounce * nx * dumping;
                        points[i].vy -= bounce * ny * dumping;
                        points[j].vx += bounce * nx * dumping;
                        points[j].vy += bounce * ny * dumping;

                        // Marcar como colisionados
                        points[i].collidedThisFrame = true;
                        points[j].collidedThisFrame = true;
                        points[i].color = sf::Color::Black;
                        points[j].color = sf::Color::Black;

                        // Incrementar el contador de colisiones
                        collisionCountThisFrame++;
                    }
                }
            }
        }

        // Renderización
        window.clear(sf::Color::White);

        // Dibuja los puntos con el color actualizado
        for (auto& point : points) {
            sf::CircleShape shape(3);
            shape.setPosition(static_cast<float>(point.x - 3), static_cast<float>(point.y - 3));
            shape.setFillColor(point.color);
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
        text.setString("Objeto: " + std::to_string(points.size()) + "\nColisiones: " + std::to_string(collisionCountThisFrame)); // + "\nCollisions: " + std::to_string(totalCollisions)
        window.draw(text);

        window.display();
    }

    return 0;
}