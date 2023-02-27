#pragma once

#include <glm/glm.hpp>
#include <iostream>

// ====================================================================
// ====================================================================

// Ray class mostly copied from Peter Shirley and Keith Morley

struct Ray {

public:

    // CONSTRUCTOR & DESTRUCTOR
    Ray() {}
    Ray(const glm::vec3& orig, const glm::vec3& dir) {
        origin = orig;
        direction = dir;
    }
    Ray(const Ray& r) { *this = r; }

    // ACCESSORS
    const glm::vec3& getOrigin() const { return origin; }
    const glm::vec3& getDirection() const { return direction; }
    glm::vec3 pointAtParameter(float t) const {
        return origin + direction * t;
    }

private:

    // REPRESENTATION
    glm::vec3 origin;
    glm::vec3 direction;
};

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& r) {
    os << "(" << r.x << ", " << r.y << ", " << r.z << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Ray& r) {
    os << "Ray <o:" << r.getOrigin() << ", d:" << r.getDirection() << ">";
    return os;
}

// ====================================================================
// ====================================================================
