
#include "simulation.hpp"
#include "oglwidget.h"
#include <iostream>

void glNormalVec3(const Vec3 &v)
{
    glNormal3f(v.x, v.y, v.z);
}

void glVertexVecVec3(const std::vector<Vec3> &v)
{
    for (const auto &vec : v)
    {
        glVertex3f(vec.x, vec.y, vec.z);
    }
}

// collision of sphere with wall
bool Wall::collide(Sphere &sphere)
{

    // cheap distance check first
    auto normal = getNormal();
    auto worldCorners = getWorldCorners();
    const auto &point = worldCorners[0];
    const auto center = sphere.getWorldPosition();
    auto radius = sphere.getRadius();
    auto sphereVelocity = sphere.getVelocity();
    auto dist = abs(normal.dot(center - point));

    if (dist > radius)
        return false;

    //double bounceFactor = calcBounceFactor(*this);

    // check for corner collision here
    for (int i = 0; i < 4; i++)
    {
        const auto &corner = worldCorners[i];
        auto vec = center - corner;
        if (vec.length() < radius)
        {
            // collision confirmed, calculate reflection
            // calculate reflection vector
            auto reflection = sphereVelocity - 2 * sphereVelocity.dot(normal) / pow(normal.length(), 2) * normal;
            sphere.setVelocity(reflection);

            // move sphere out of corner
            Vec3 move = reflection.normalized() * (radius - dist + 0.001);
            sphere.move(move);
            return true;
        }
    }

    // check if sphere collides with edge

    for (int i = 0; i < 4; i++)
    {
        auto &corner1 = worldCorners[i];
        auto &corner2 = worldCorners[(i + 1) % 4];
        auto edge = corner2 - corner1;
        auto edgeNormalized = edge.normalized();

        auto ca = center - corner1;

        // calculate distance to edge
        // distance from corner1 to closest point on edge
        auto edgedist = edgeNormalized.dot(ca);
        auto closestPoint = corner1 + edgedist * edgeNormalized;
        double cpdist = closestPoint.getDistance(center);
        if (cpdist > radius)
            continue;

        // calculate closest point on edge to sphere center
        // double t = ca.dot(edge) / edge.dot(edge);
        // auto p = corner1 + t * edge;
        auto &p = closestPoint;

        // check if collision point is between both worldCorners by checking if distance |p-corner1| + |p-corner2| is equal to |corner1-corner2|
        auto dist1 = p.getDistance(corner1);
        auto dist2 = p.getDistance(corner2);
        auto dist3 = corner1.getDistance(corner2);
        constexpr double tolerance = 0.01;
        if (dist1 + dist2 > dist3 + tolerance)
            continue;

        // collision confirmed, calculate reflection
        // calculate reflection vector
        auto collToCenter = center - p;
        collToCenter = collToCenter.normalized();
        auto reflection = sphereVelocity - 2 * sphereVelocity.dot(collToCenter) * collToCenter;
        sphere.setVelocity(reflection);

        // move sphere out of wall
        Vec3 move = reflection.normalized() * (radius - abs(cpdist) + 0.001) * (1 / collToCenter.dot(reflection.normalized()));
        sphere.move(move);
    }

    // check if sphere collides with face
    // already in range of plane, check if collisionpoint is inside face
    // face normal: normal
    // face point: worldCorners[0]

    // calculate closest point on plate to sphere center

    // use non abs distance to get direction
    auto newDist = normal.dot(center - point);
    auto p = center - newDist * normal;

    // check if collision point is between all worldCorners

    // convert wall to 2d rectangle
    // wall[0] = 0/0
    // wall[1] = 0/1
    // wall[2] = 1/1
    // wall[3] = 1/0

    // create vectors to span rectangle
    auto v1 = worldCorners[1] - worldCorners[0];
    auto v2 = worldCorners[3] - worldCorners[0];

    // normalize
    v1 = v1.normalized();
    v2 = v2.normalized();

    // create vector to span z axis (this should just be the plane normal?)
    auto n = v1.cross(v2).normalized();

    // convert a point to 2d:
    // px = v1.dot(p - worldCorners[0])
    // py = v2.dot(p - worldCorners[0])
    // pz = n.dot(p - worldCorners[0])
    // pz should be 0 and can be ignored, px and py form the 2d point

    // max values for height and width
    auto topRight = worldCorners[2] - worldCorners[0];
    auto trX = v1.dot(topRight);
    auto trY = v2.dot(topRight);
    // auto trZ = n.dot(topRight);

    // vector from corner to point
    auto pnew = p - worldCorners[0];

    // calculate px, py and pz
    auto px = v1.dot(pnew) / trX;
    auto py = v2.dot(pnew) / trY;
    auto pz = n.dot(pnew);

    // check if pz is 0 with tolerance
    // should always be near 0 since we already checked distance
    if (pz > 0.01 || pz < -0.01)
        return false;

    // check if px and py are between 0 and trX and trY
    if (px < 0 || px > 1 || py < 0 || py > 1)
        return false;

    // barycentric approach
    // works, but currently not used
    // is also slower
    // create two triangles from worldCorners
    /*
    bool isInside = false;
    for (int i = 0; i < 3; i += 2)
    {
        // calculate using barycentric coordinates
        auto a = worldCorners[i];
        auto b = worldCorners[i + 1];
        auto c = worldCorners[(i + 2) % 4];

        // vectors from a to b and a to c and a to p
        Vec3 v0 = c - a;
        Vec3 v1 = b - a;
        Vec3 v2 = p - a;

        // dot products
        double dot00 = v0.dot(v0);
        double dot01 = v0.dot(v1);
        double dot02 = v0.dot(v2);
        double dot11 = v1.dot(v1);
        double dot12 = v1.dot(v2);

        // inverse denominator to avoid division later
        double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);

        // barycentric coordinates
        double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        // check if point is in triangle
        double tolerance = 0;
        if ((u >= -tolerance) && (v >= -tolerance) && (u + v <= 1.0 + tolerance))
        {
            isInside = true;
            break;
        }
    }

    if (!isInside)
        return false;
    */

    // collision confirmed, calculate reflection
    // calculate reflection vector
    // instead of normal use collToCenter
    // this is the same direction as the normal, but it can be negative if the sphere is on the other side of the wall
    auto collToCenter = center - p;
    collToCenter = collToCenter.normalized();
    auto reflection = sphereVelocity - 2 * sphereVelocity.dot(collToCenter) / pow(collToCenter.length(), 2) * collToCenter;
    sphere.setVelocity(reflection);
    // move sphere out of wall
    Vec3 move = reflection.normalized() * (radius - dist + 0.001) * (1 / collToCenter.dot(reflection.normalized()));

    sphere.move(move);

    return true;
}

// applies new velocity to object with consideration of bounce or friction
void SimObject::applyCollisionVelocity(const Vec3& newVelocity, const Vec3& otherNormal, const SimObject& other) {

    // check if collision is a bounce or roll
    double dot = newVelocity.normalized().dot(otherNormal);
    if (true || newVelocity.lengthSquared()<0.1) {
        std::cout << "Rolling" << std::endl;
        // roll
        // Ff = mu * N
        // force is opposite to velocity
        // apply friction
        double ff = other.frictionCoefficient * this->getMass() * 9.81;
        constexpr double dt = 1.0 / 60.0;
        double acc = ff / this->getMass();
        
        double fVel = acc * dt;
        if (fVel > newVelocity.length()) {
            std::cout << "Setting vel to 0" << std::endl;
            this->velocity = Vec3(0, 0, 0);
            return;
        }

        // calculate velocity direction without bounce (orthogonal to normal)
        auto velDir = (newVelocity.normalized() - otherNormal * dot);

        // apply friction
        // force is opposite to velocity
        this->velocity = velDir * (newVelocity.length()-fVel);
    }
    else {
        std::cout << dot << std::endl;
        // bounce
        // test: only reduce velocity in normal dir
        //this->velocity = newVelocity - (1 -  calcBounceFactor(other)) * dot * otherNormal * newVelocity.length();
        this->velocity = newVelocity;
        this->velocity.y *= calcBounceFactor(other);
    }

}

// collision of sphere with sphere
void Sphere::bounce(Sphere &other)
{
    // calculate reflection vector
    auto mass1 = this->getMass();
    auto mass2 = other.getMass();
    auto v1 = this->getVelocity();
    auto v2 = other.getVelocity();
    auto p1 = this->getWorldPosition();
    auto p2 = other.getWorldPosition();

    // calculate new velocities
    // collision point based on radius
    auto coll = (p1 - p2).normalized() * this->getRadius();
    coll += p1;
    // calculate new velocities
    // new vel = old vel - 2 * massFactor * collisionFactor / distance^2 * collisionVector
    auto v1f = v1 - 2 * mass2 / (mass1 + mass2) * (v1 - v2).dot(p1 - coll) / pow((p1 - coll).length(), 2) * (p1 - coll);
    auto v2f = v2 - 2 * mass1 / (mass1 + mass2) * (v2 - v1).dot(p2 - coll) / pow((p2 - coll).length(), 2) * (p2 - coll);

    double bounce = calcBounceFactor(other);

    this->setVelocity(v1f*bounce);
    other.setVelocity(v2f*bounce);

    // move spheres out of each other
    auto dist = (this->getRadius() + other.getRadius()) * 1.001;
    auto vec = this->getWorldPosition() - other.getWorldPosition();
    auto moveDirection = vec.normalized();
    auto move = moveDirection * (dist - vec.length());
    this->move(move);
    other.move(move * -1);
}

double Vec3::getDistance(const Vec3 &other)
{
    return sqrt(pow(this->x - other.x, 2) + pow(this->y - other.y, 2) + pow(this->z - other.z, 2));
}

Vec3 Vec3::getNormal(const Vec3 &other1, const Vec3 &other2)
{
    Vec3 v1 = other1 - *this;
    Vec3 v2 = other2 - *this;
    return v1.cross(v2).normalized();
}

double SimObject::calcBounceFactor(const SimObject &other)
{

    double factor = (other.bounceFactor * this->bounceFactor);
    return factor;
}

void SimObject::tick(double time)
{
    // tick children
    for (SimObject *child : children)
    {
        child->tick(time);
    }
}

void SimObject::setPosition(Vec3 position)
{
    this->position = position;
    auto myPos = this->getWorldPosition();
    for (SimObject *child : children)
    {
        child->setWorldPosition(myPos);
    }
}

bool SimObject::collide(Sphere& sphere) {
    // check collision with children
    bool collided = false;
    for (SimObject *child : children)
    {
        if(child->collide(sphere)) collided = true;
    }
    return collided;
}

void SimObject::setWorldPosition(Vec3 position)
{
    this->worldPosition = position;
    auto myPos = this->getWorldPosition();
    for (SimObject *child : children)
    {
        child->setWorldPosition(myPos);
    }
}

void SimObject::addChild(SimObject *child)
{
    children.push_back(child);
    child->parent = this;
    child->setWorldPosition(this->getWorldPosition());
}

void SimObject::draw()
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glMultMatrixf(rotation.data());

    // draw children
    for (SimObject *child : children)
    {
        child->draw();
    }

    glPopMatrix();
}

Triangle::Triangle(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3)
{
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
}

void Triangle::draw()
{
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glTranslatef(position.x, position.y, position.z);
    glColor3f(1.0f, 0.0f, 0.0f);
    glNormalVec3(getNormal());
    glVertex3f(p1.x, p1.y, p1.z);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(p2.x, p2.y, p2.z);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(p3.x, p3.y, p3.z);
    glEnd();
    glPopMatrix();

    SimObject::draw();
}

bool Triangle::collide(Sphere &sphere)
{
    // cheap distance check first
    auto normal = getNormal();
    auto worldCorners = getWorldCorners();
    const auto &point = worldCorners[0];
    const auto center = sphere.getWorldPosition();
    auto radius = sphere.getRadius();
    auto sphereVelocity = sphere.getVelocity();
    auto dist = abs(normal.dot(center - point));

    if (dist > radius)
        return false;

    double bounceFactor = sphere.calcBounceFactor(*this);

    // check for corner collision here
    if (!faceCollisionOnly)
        for (int i = 0; i < 3; i++)
        {
            const auto &corner = worldCorners[i];
            auto vec = center - corner;
            if (vec.length() < radius)
            {
                // collision confirmed, calculate reflection
                // calculate reflection vector
                auto reflection = sphereVelocity - 2 * sphereVelocity.dot(normal) / pow(normal.length(), 2) * normal;
                sphere.setVelocity(reflection*bounceFactor);

                // move sphere out of corner
                Vec3 move = reflection.normalized() * (radius - dist + 0.001);
                sphere.move(move);
                return true;
            }
        }

    // check if sphere collides with edge
    if (!faceCollisionOnly)
        for (int i = 0; i < 3; i++)
        {
            auto &corner1 = worldCorners[i];
            auto &corner2 = worldCorners[(i + 1) % 3];
            auto edge = corner2 - corner1;
            auto edgeNormalized = edge.normalized();

            auto ca = center - corner1;

            // calculate distance to edge
            // distance from corner1 to closest point on edge
            auto edgedist = edgeNormalized.dot(ca);
            auto closestPoint = corner1 + edgedist * edgeNormalized;
            double cpdist = closestPoint.getDistance(center);
            if (cpdist > radius)
                continue;

            // calculate closest point on edge to sphere center
            // double t = ca.dot(edge) / edge.dot(edge);
            // auto p = corner1 + t * edge;
            auto &p = closestPoint;

            // check if collision point is between both worldCorners by checking if distance |p-corner1| + |p-corner2| is equal to |corner1-corner2|
            auto dist1 = p.getDistance(corner1);
            auto dist2 = p.getDistance(corner2);
            auto dist3 = corner1.getDistance(corner2);
            constexpr double tolerance = 0.01;
            if (dist1 + dist2 > dist3 + tolerance)
                continue;

            // collision confirmed, calculate reflection
            // calculate reflection vector
            auto collToCenter = center - p;
            collToCenter = collToCenter.normalized();
            auto reflection = sphereVelocity - 2 * sphereVelocity.dot(collToCenter) * collToCenter;
            sphere.setVelocity(reflection*bounceFactor);

            // move sphere out of wall
            Vec3 move = reflection.normalized() * (radius - abs(cpdist) + 0.001) * (1 / collToCenter.dot(reflection.normalized()));
            sphere.move(move);
        }

    // check if sphere collides with face
    // already in range of plane, check if collisionpoint is inside face
    // face normal: normal
    // face point: worldCorners[0]

    // calculate closest point on plate to sphere center

    // use non abs distance to get direction
    auto newDist = normal.dot(center - point);
    auto p = center - newDist * normal;

    // check if collision point is between all worldCorners

    // barycentric approach

    // calculate using barycentric coordinates
    auto &a = worldCorners[0];
    auto &b = worldCorners[1];
    auto &c = worldCorners[2];

    // vectors from a to b and a to c and a to p
    Vec3 v0 = c - a;
    Vec3 v1 = b - a;
    Vec3 v2 = p - a;

    // dot products
    double dot00 = v0.dot(v0);
    double dot01 = v0.dot(v1);
    double dot02 = v0.dot(v2);
    double dot11 = v1.dot(v1);
    double dot12 = v1.dot(v2);

    // inverse denominator to avoid division later
    double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);

    // barycentric coordinates
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // check if point is in triangle
    double tolerance = 0;
    if (!((u >= -tolerance) && (v >= -tolerance) && (u + v <= 1.0 + tolerance)))
    {
        return false;
    }

    // collision confirmed, calculate reflection
    // calculate reflection vector
    // instead of normal use collToCenter
    // this is the same direction as the normal, but it can be negative if the sphere is on the other side of the wall
    auto collToCenter = center - p;
    collToCenter = collToCenter.normalized();
    auto reflection = sphereVelocity - 2 * sphereVelocity.dot(collToCenter) / pow(collToCenter.length(), 2) * collToCenter;
    sphere.applyCollisionVelocity(reflection, normal, *this);
    // move sphere out of wall
    Vec3 move = reflection.normalized() * (radius - dist + 0.001) * (1 / collToCenter.dot(reflection.normalized()));

    sphere.move(move);

    return true;
}

std::vector<Vec3> Triangle::getWorldCorners()
{
    auto worldPos = getWorldPosition();
    return {
        worldPos + p1,
        worldPos + p2,
        worldPos + p3,
    };
}

Plane::Plane(Vec3 normal, Vec3 point) : normal(normal), point(point)
{
    this->normal = this->normal.normalized();
}

double Plane::getDistance(const Vec3 &other)
{
    return abs((other - point).dot(normal));
}

Vec3 Plane::bounce(const Vec3 &v)
{
    // calculate reflection vector
    // skalarprodukt
    double dot = v.dot(normal);
    // reflektionsvektor
    Vec3 reflection = v - 2 * dot * normal;
    return reflection;
}

Wall::Wall(double x1, double z1, double x2, double z2) : SimObject()
{
    constexpr double HEIGHT = 2;
    corners.push_back(Vec3(x1, 0, z1));
    corners.push_back(Vec3(x1, HEIGHT, z1));
    corners.push_back(Vec3(x2, HEIGHT, z2));
    corners.push_back(Vec3(x2, 0, z2));
}

void Wall::draw()
{

    // draw wall
    glPushMatrix();
    glTranslated(position.x, position.y, position.z);
    // wall defined by 4 corners
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_QUADS);
    glNormalVec3(getNormal());

    // dont need worldCorners, because relative position is already applied in parent matrix
    auto corners = getCorners();
    glVertexVecVec3(corners);

    glEnd();
    glPopMatrix();

    SimObject::draw();
}

Wall::Wall(const Vec3 &corner1, const Vec3 &corner2, const Vec3 &corner3, const Vec3 &corner4) : SimObject()
{
    corners.push_back(corner1);
    corners.push_back(corner2);
    corners.push_back(corner3);
    corners.push_back(corner4);
}

std::vector<Vec3> Wall::getWorldCorners()
{
    auto wPos = this->getWorldPosition();
    return {
        corners[0] + wPos,
        corners[1] + wPos,
        corners[2] + wPos,
        corners[3] + wPos};
}

void Sphere::draw()
{
    glPushMatrix();

    // position
    glTranslatef(position.x, position.y, position.z);

    // draw axis if enabled
    if (OGLWidget::showAxis)
    {
        // draw movement vector
        auto embiggenedVelocity = velocity.normalized() * radius * 2;
        glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertexNPoints(Vec3(0, 0, 0), embiggenedVelocity);
        glEnd();

        // draw floor normal
        auto embiggenedFloorNormal = currentFloorNormal.normalized() * radius * 2;
        glBegin(GL_LINES);
        glColor3f(0, 1, 0);
        glVertexNPoints(Vec3(0, 0, 0), embiggenedFloorNormal);
        glEnd();

        // draw rotation axis
        auto embiggenedRotationAxis = currentFloorNormal.cross(velocity).normalized() * radius * 2;
        glBegin(GL_LINES);
        glColor3f(0, 0, 1);
        glVertexNPoints(Vec3(0, 0, 0), embiggenedRotationAxis);
        glEnd();
    }

    // rotation

    // glRotatef(rotation.angle * 180.0 / PI, rotation.axis.x, rotation.axis.y, rotation.axis.z);
    glMultMatrixf(rotation.data());
    // scale with radius
    glScalef(radius, radius, radius);

    // color
    glColor3f(color.x, color.y, color.z);

    for (float beta = 0.0; beta <= PI - 0.0001; beta += PI / resolution)
    {
        int step = round(beta * resolution / PI + 0.0001);
        switch (step % 2)
        {
        case 0:
            glColor3f(color.x, color.y, color.z);
            break;
        case 1:
            glColor3f(1, 0.7, 1);
            break;
        }

        glBegin(GL_TRIANGLE_STRIP);
        for (float alpha = 0.0; alpha < 2.01 * PI; alpha += PI / resolution)
        {
            float x = sin(beta) * cos(alpha);
            float y = sin(beta) * sin(alpha);
            float z = cos(beta);

            glNormalVec3(Vec3(x, y, z));
            // glColor3ub( rand()%255, rand()%255, rand()%255 );
            glVertex3f(x, y, z);
            x = sin(beta + PI / resolution) * cos(alpha);
            y = sin(beta + PI / resolution) * sin(alpha);
            z = cos(beta + PI / resolution);

            glNormalVec3(Vec3(x, y, z));
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glPopMatrix();

    SimObject::draw();
}

void Sphere::move(Vec3 v)
{
    if (v.lengthSquared() == 0.0)
        return;
    Vec3 &floor = this->getFloorNormal();
    if (floor.lengthSquared() < 0.01)
    {
        // no floor
        setPosition(this->getPosition() + v);
        return;
    }

    // calculate axis
    auto vnorm = v.normalized();
    auto cross = vnorm.cross(floor);
    if (cross.lengthSquared() < 0.00001)
    {
        // no rotation
        setPosition(this->getPosition() + v);
        return;
    }
    auto rot = cross.normalized();

    // calculate angle
    // idk why -
    auto angle = -360.0 * v.length() * (1-v.dot(getFloorNormal())) / (2.0 * PI * radius);
    QMatrix4x4 rotMatrix;
    rotMatrix.rotate(angle, rot.x, rot.y, rot.z);
    rotMatrix *= rotation;
    rotation = rotMatrix;
    setPosition(this->getPosition() + v);
}

void Sphere::moveTo(Vec3 v)
{
    auto diff = v - getWorldPosition();
    move(diff);
}

double Sphere::getMass()
{
    return 4.0 / 3.0 * PI * pow(radius, 3) * density;
}

Box::Box(const std::vector<double> &xnzn) : SimObject()
{
    // xnzn = x1, z1, x2, z2, ...
    // create walls from xnzn
    for (size_t i = 0; i < xnzn.size(); i += 2)
    {
        walls.push_back(Wall(xnzn[i], xnzn[i + 1], xnzn[(i + 2) % xnzn.size()], xnzn[(i + 3) % xnzn.size()]));
        outerWallCount++;
    }
}

void Box::draw()
{
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    // draw floor

    glBegin(GL_TRIANGLE_FAN);
    glNormalVec3(Vec3(0, 1, 0));
    glColor3f(0.5, 0.5, 0.5);
    glVertex3f(0, 0, 0);
    for (size_t i = 0; i <= outerWallCount; i++)
    {
        const auto &corner = walls[i % outerWallCount].getCorners()[0];
        glVertexNPoints(corner);
    }
    glEnd();

    // draw walls
    for (Wall &wall : walls)
    {
        wall.draw();
    }

    glEnd();
    glPopMatrix();
}
