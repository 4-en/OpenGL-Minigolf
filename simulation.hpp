
#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <functional>
#include <math.h>

#include "QMatrix4x4"

// Helper functions
inline double randRange(double min, double max)
{
    return min + (max - min) * (double)rand() / RAND_MAX;
}

constexpr double PI = 3.14159265358979323846;
// Helper function to draw multiple points
// Usage: glVertexNPoints(v1, v2, v3, ...)
// using a fold expression


// This is a 3D vector class
class Vec3 {
public:
    double x, y, z;
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    Vec3() : x(0), y(0), z(0) {}
    Vec3(double xyz) : x(xyz), y(xyz), z(xyz) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    void operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    void operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; }
    Vec3 operator*(double s) const { return Vec3(x * s, y * s, z * s); }
    void operator*=(double s) { x *= s; y *= s; z *= s; }
    Vec3 operator/(double s) const { return Vec3(x / s, y / s, z / s); }
    void operator/=(double s) { x /= s; y /= s; z /= s; }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    bool operator==(const Vec3& v) const { return (x==v.x && y==v.y && z==v.z); }
    // Dot product
    double dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    // Cross product
    Vec3 cross(const Vec3& v) const { return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    double length() const { return sqrt(x * x + y * y + z * z); }
    double lengthSquared() const { return x * x + y * y + z * z; }
    // Normalize
    Vec3 normalized() const { return *this / length(); }
    // Get distance between two points
    double getDistance(const Vec3& other);
    // Get normal of a plane defined by this location and two directions
    Vec3 getNormal(const Vec3& other1, const Vec3& other2);
    friend Vec3 operator*(double s, const Vec3& v) { return v * s; }
};


template <typename... Vec3>
void glVertexNPoints(const Vec3 &...v)
{
    (glVertex3f(v.x, v.y, v.z), ...);
}




// This is a plane class
// It is defined by a normal and a point
class Plane {
public:
    Vec3 normal;
    Vec3 point;
    Plane(Vec3 normal, Vec3 point);
    Plane() : normal(0,1,0), point(0,0,0) {}
    double getDistance(const Vec3& other);
    Vec3 bounce(const Vec3& v);

};

class Sphere;

// A simulation object is an abstract class used to represent objects in the simulation
class SimObject {
protected:
    double bounceFactor = 0.8;
    Vec3 center;
    QMatrix4x4 rotation;
    Vec3 velocity;
    Vec3 color;
    double density=1.0;
    std::vector<SimObject*> children;

public:
    SimObject() : center(0), rotation(), velocity(0), color(1,0,0), density(1) {}
    SimObject(Vec3 center, double density=1) : center(center), rotation(), velocity(0), color(1,0,0), density(density) {}
    void setCenter(Vec3 center) { this->center = center; }
    void setDensity(double density) { this->density = density; }
    void setRotation(const QMatrix4x4& rotation) { this->rotation = rotation; }
    void setVelocity(Vec3 velocity) { this->velocity = velocity; }
    void setColor(Vec3 color) { this->color = color; }
    Vec3& getCenter() { return center; }
    double getDensity() { return density; }
    QMatrix4x4& getRotation() { return rotation; }
    Vec3& getVelocity() { return velocity; }
    Vec3& getColor() { return color; }
    double getBounceFactor() { return bounceFactor; }
    void setBounceFactor(double bounceFactor) { this->bounceFactor = bounceFactor; }
    double calcBounceFactor(SimObject& other);
    void addChild(SimObject* child) { children.push_back(child); }
    std::vector<SimObject*>& getChildren() { return children; }
    virtual bool collide(Sphere& sphere) { return false; };

    virtual void draw();
    virtual double getMass() { return static_cast<double>(LLONG_MAX); }
};

// Predefine Sphere class to use in Wall class
class Sphere;

// A wall is defined by four corners
// acts like a finite plane
class Wall : public SimObject
{
protected:
    std::vector<Vec3> corners;

public:
    Wall(const Vec3& corner1, const Vec3& corner2, const Vec3& corner3, const Vec3& corner4);
    Wall() : Wall(Vec3(-1,0,-1), Vec3(1,0,-1), Vec3(1,0,1), Vec3(-1,0,1)) {}
    Wall(double x1, double z1, double x2, double z2);

    void draw();
    double getMass() { return 99999999999.9;}
    bool collide(Sphere& sphere);
    Vec3 getNormal() { return corners[0].getNormal(corners[1], corners[2]); }
    std::vector<Vec3>& getCorners() { return corners; }
};

// A sphere is defined by a center and a radius
class Sphere : public SimObject
{
protected:
    double radius;
    // Steps used to draw the sphere
    int resolution;
    // Normal of the floor, used for rolling
    Vec3 currentFloorNormal = Vec3(0,1,0);

public:
    Sphere() : SimObject(), radius(1), resolution(10) {}
    Sphere(Vec3 center, double radius, int resolution=10) : SimObject(center), radius(radius), resolution(resolution) {}
    void setRadius(double radius) { this->radius = radius; }
    void setResolution(int resolution) { this->resolution = resolution; }
    double getRadius() { return radius; }
    int getResolution() { return resolution; }
    void setFloorNormal(Vec3 normal) { currentFloorNormal = normal; }
    Vec3& getFloorNormal() { return currentFloorNormal; }
    void draw();
    void move(Vec3 v);
    void moveTo(Vec3 v);
    double getMass();
    void bounce(Sphere& other);

};

// The box in which the simulation takes place
// It is defined by a vector of walls
class Box : public SimObject
{
protected:
    std::vector<Wall> walls;
    size_t outerWallCount = 0;

public:
    Box() : SimObject() {}
    Box(const std::vector<double>& xnzn);



    std::vector<Wall>& getWalls() { return walls; }
    void draw();
    double getMass() { return 99999999999.9;}
    size_t getOuterWallCount() { return outerWallCount; }
};




#endif // SIMULATION_HPP
