#include "Circle.h"

class QuadTree {
private:
    static const int DEFAULT_CAPACITY = 10;
    static const int MAX_DEPTH = 10;

    Rectangle boundary;
    int capacity;
    std::vector<Point> points;
    bool divided;
    int depth;
    
    QuadTree* northeast;
    QuadTree* northwest;
    QuadTree* southeast;
    QuadTree* southwest;

public:
    QuadTree(const Rectangle& _boundary, int _capacity = DEFAULT_CAPACITY, int _depth = 0)
    : boundary(_boundary), capacity(_capacity), divided(false), depth(_depth),
      northeast(nullptr), northwest(nullptr), southeast(nullptr), southwest(nullptr) {}


    // Getter methods
    QuadTree* getNortheast() const { return northeast; }
    QuadTree* getNorthwest() const { return northwest; }
    QuadTree* getSoutheast() const { return southeast; }
    QuadTree* getSouthwest() const { return southwest; }
    bool isDivided() const { return divided; }
    Rectangle getBoundary() const { return boundary; }
    std::vector<Point> getPoints() const { return points; }
    int getCapacity() const {return capacity; }

    std::vector<QuadTree*> getChildren() const {
        std::vector<QuadTree*> children;
        if (divided) {
            children.push_back(northeast);
            children.push_back(northwest);
            children.push_back(southeast);
            children.push_back(southwest);
        }
        return children;
    }

    void clear() {
        points.clear();
        if (divided) {
            delete northeast; northeast = nullptr;
            delete northwest; northwest = nullptr;
            delete southeast; southeast = nullptr;
            delete southwest; southwest = nullptr;
            divided = false;
        }
    }


    static QuadTree* create(const Rectangle& boundary, int capacity = DEFAULT_CAPACITY) {
        return new QuadTree(boundary, capacity);
    }

    static QuadTree* create(double x, double y, double w, double h, int capacity = DEFAULT_CAPACITY) {
        return new QuadTree(Rectangle(x, y, w, h), capacity);
    }

    void subdivide() {
        northeast = new QuadTree(boundary.subdivide("ne"), capacity, depth + 1);
        northwest = new QuadTree(boundary.subdivide("nw"), capacity, depth + 1);
        southeast = new QuadTree(boundary.subdivide("se"), capacity, depth + 1);
        southwest = new QuadTree(boundary.subdivide("sw"), capacity, depth + 1);

        divided = true;

        for (const auto& p : points) {
            bool inserted =
                northeast->insert(p) ||
                northwest->insert(p) ||
                southeast->insert(p) ||
                southwest->insert(p);

            if (!inserted) {
                // Manejo de error o lanzamiento de excepción
            }
        }

        points.clear();
    }

    bool insert(const Point& point) {
        if (!boundary.contains(point)) {
            return false;
        }

        if (!divided) {
            if (points.size() < static_cast<std::vector<Point>::size_type>(capacity) || depth == MAX_DEPTH) {
                points.push_back(point);
                return true;
            }

            subdivide();
        }

        return
            northeast->insert(point) ||
            northwest->insert(point) ||
            southeast->insert(point) ||
            southwest->insert(point);
    }

    std::vector<Point> query(const Rectangle& range) {
        std::vector<Point> found;

        if (!range.intersects(boundary)) {
            return found;
        }

        if (divided) {
            std::vector<QuadTree*> children = getChildren();
            for (auto child : children) {
                std::vector<Point> childPoints = child->query(range);
                found.insert(found.end(), childPoints.begin(), childPoints.end());
            }
            return found;
        }

        for (const auto& p : points) {
            if (range.contains(p)) {
                found.push_back(p);
            }
        }

        return found;
    }

    void delete_in_range(const Rectangle& range) {
        if (divided) {
            northeast->delete_in_range(range);
            northwest->delete_in_range(range);
            southeast->delete_in_range(range);
            southwest->delete_in_range(range);
        }

        // Eliminar puntos dentro del rango
        points.erase(std::remove_if(points.begin(), points.end(), [&range](const Point& p) {
            return range.contains(p);
        }), points.end());
    }

    std::vector<Point> closest(const Point& searchPoint, int maxCount = 1, double maxDistance = std::numeric_limits<double>::infinity()) {
        // &searchPoint == nullptr :::: searchPoint is assigned by reference, so it can't be nullptr

        double sqMaxDistance = std::pow(maxDistance, 2);
        return k_nearest(searchPoint, maxCount, sqMaxDistance, 0, 0).found;
    }

    struct SearchResult {
        std::vector<Point> found;
        double furthestSqDistance;
    };

    SearchResult k_nearest(const Point& searchPoint, int maxCount, double sqMaxDistance, double furthestSqDistance, int foundSoFar) {
        SearchResult result;

        if (divided) {
            std::vector<QuadTree*> sortedChildren = getChildren();
            std::sort(sortedChildren.begin(), sortedChildren.end(), [&searchPoint](QuadTree* a, QuadTree* b) {
                return a->boundary.sqDistanceFrom(searchPoint) < b->boundary.sqDistanceFrom(searchPoint);
            });

            for (auto child : sortedChildren) {
                double sqDistance = child->boundary.sqDistanceFrom(searchPoint);
                if (sqDistance > sqMaxDistance) {
                    continue;
                } else if (foundSoFar < maxCount || sqDistance < furthestSqDistance) {
                    SearchResult childResult = child->k_nearest(searchPoint, maxCount, sqMaxDistance, furthestSqDistance, foundSoFar);
                    std::vector<Point> childPoints = childResult.found;
                    result.found.insert(result.found.end(), childPoints.begin(), childPoints.end());
                    foundSoFar += childPoints.size();
                    furthestSqDistance = childResult.furthestSqDistance;
                }
            }
        } else {
            std::vector<Point> sortedPoints = points;
            std::sort(sortedPoints.begin(), sortedPoints.end(), [&searchPoint](const Point& a, const Point& b) {
                return a.sqDistanceFrom(searchPoint) < b.sqDistanceFrom(searchPoint);
            });

            for (const auto& p : sortedPoints) {
                double sqDistance = p.sqDistanceFrom(searchPoint);
                if (sqDistance > sqMaxDistance) {
                    continue;
                } else if (foundSoFar < maxCount || sqDistance < furthestSqDistance) {
                    result.found.push_back(p);
                    furthestSqDistance = std::max(sqDistance, furthestSqDistance);
                    foundSoFar++;
                }
            }
        }

        result.found.erase(std::unique(result.found.begin(), result.found.end()), result.found.end());

        result.furthestSqDistance = std::sqrt(furthestSqDistance);

        return result;
    }

    void for_each(const std::function<void(const Point&)>& fn) {
        if (divided) {
            for (auto child : getChildren()) {
                child->for_each(fn);
            }
        } else {
            for (const auto& p : points) {
                fn(p);
            }
        }
    }

    QuadTree* filter(const std::function<bool(const Point&)>& fn) {
        QuadTree* filtered = new QuadTree(boundary, capacity);

        auto insert_filtered = [filtered, fn](const Point& point) {
            if (fn(point)) {
                filtered->insert(point);
            }
        };

        for_each(insert_filtered);

        return filtered;
    }

    QuadTree* merge(QuadTree* other, int capacity) {
        double left = std::min(boundary.left, other->boundary.left);
        double right = std::max(boundary.right, other->boundary.right);
        double top = std::min(boundary.top, other->boundary.top);
        double bottom = std::max(boundary.bottom, other->boundary.bottom);

        double height = bottom - top;
        double width = right - left;

        double mid_x = left + width / 2;
        double mid_y = top + height / 2;

        Rectangle mergedBoundary(mid_x, mid_y, width, height);
        QuadTree* result = new QuadTree(mergedBoundary, capacity);

        auto insert_point = [result](const Point& point) {
            result->insert(point);
        };

        for_each(insert_point);
        other->for_each(insert_point);

        return result;
    }

    int length() const {
        int length = points.size();
        if (divided) {
            for (auto child : getChildren()) {
                length += child->length();
            }
        }
        return length;
    }

    // Método para verificar si un punto ha salido de su región actual
    bool pointMoved(const Point& point) {
        // Verificar si el punto todavía está dentro de los límites de este QuadTree
        return !boundary.contains(point);
    }

    // Método para eliminar un punto específico del QuadTree
    bool remove(const Point& point) {
        // Esto es ineficiente: busca en el vector y elimina el punto si lo encuentra
        auto it = std::find(points.begin(), points.end(), point);
        if (it != points.end()) {
            points.erase(it);
            return true;  // Punto encontrado y eliminado
        }

        // Si el QuadTree está dividido, intenta eliminar el punto de los nodos hijos
        if (divided) {
            return northeast->remove(point) ||
                   northwest->remove(point) ||
                   southeast->remove(point) ||
                   southwest->remove(point);
        }

        return false;  // Punto no encontrado en este nodo
    }

    ~QuadTree() {
        clear();
        // Asegúrate de que también se establezcan a nullptr después de borrar.
        northeast = nullptr;
        northwest = nullptr;
        southeast = nullptr;
        southwest = nullptr;
    }

};