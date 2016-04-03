#pragma once

#include <cmath>
#include <boost/optional.hpp>
#include <QtCore/QPointF>
#include <protobuf/Point.pb.h>
#include <sstream>

namespace Geometry2d {
/**
Simple class to represent a point in 2d space. Uses floating point coordinates
*/
class Point {
public:
    const float& x() const { return _x; }
    const float& y() const { return _y; }
    float& x() { return _x; }
    float& y() { return _y; }

    /**
    sets the point to x,y
    @param x the x coordinate
    @param y the y coordinate
    */
    Point(float x = 0, float y = 0) : _x(x), _y(y) {}

    /**
     * Implicit constructor for creating a Point from a Packet::Point
     */
    Point(const Packet::Point& other) : Point(other.x(), other.y()) {}

    /**
     * Implicit constructor for creating a Point from a QPointF
     */
    Point(const QPointF& other) : Point(other.x(), other.y()) {}

    /**
     * Implicit constructor for creating a Point from a QPoint
     */
    Point(const QPoint& other) : Point(other.x(), other.y()) {}

    /**
     * to draw stuff and interface with QT
     */
    QPointF toQPointF() const { return QPointF(x(), y()); }

    operator Packet::Point() const {
        Packet::Point out;
        out.set_x(x());
        out.set_y(y());
        return out;
    }
    /**
     * does vector addition
     * adds the + operator, shorthand
     */
    Point operator+(Point other) const {
        return Point(x() + other.x(), y() + other.y());
    }

    /**
     * see operator+
     * does vector division, note the operator
     */
    Point operator/(Point other) const {
        return Point(x() / other.x(), y() / other.y());
    }

    /**
     * @returns (x*x,y*y)
     */
    Point operator*(Point other) const {
        return Point(x() * other.x(), y() * other.y());
    }

    /**
     * see operator+
     * does vector subtraction, note the operator
     * without parameter, it is the negative
     */
    Point operator-(Point other) const {
        return Point(x() - other.x(), y() - other.y());
    }

    /**
     * multiplies the point by a -1 vector
     */
    Point operator-() const { return Point(-x(), -y()); }

    /**
     * see operator+
     * this modifies the value instead of returning a new value
     */
    Point& operator+=(Point other) {
        _x += other.x();
        _y += other.y();

        return *this;
    }

    /**
     * see operator-
     * this modifies the value instead of returning a new value
     */
    Point& operator-=(Point other) {
        _x -= other.x();
        _y -= other.y();

        return *this;
    }

    /**
     * see operator*
     * this modifies the value instead of returning a new value
     */
    Point& operator*=(float s) {
        _x *= s;
        _y *= s;

        return *this;
    }

    /**
     * see operator/
     * this modifies the value instead of returning a new value
     */
    Point& operator/=(float s) {
        _x /= s;
        _y /= s;

        return *this;
    }

    /**
     * adds the / operator for vectors
     *  scalar division
     */
    Point operator/(float s) const { return Point(x() / s, y() / s); }
    /**
     * adds the * operator for vectors
     * scalar multiplication
     */
    Point operator*(float s) const { return Point(x() * s, y() * s); }

    /**
     * compares two points to see if both x and y are the same
     * adds the == operator
     */
    bool operator==(Point other) const {
        return _x == other.x() && _y == other.y();
    }

    /**
     * this is the negation of operator operator !=
     */
    bool operator!=(Point other) const {
        return x() != other.x() || y() != other.y();
    }

    /**
    computes the dot product of this point and another.
    behaves as if the points were 2d vectors
    @param p the second point
    @return the dot product of the two
    */
    float dot(Point p) const { return x() * p.x() + y() * p.y(); }

    /**
    computes the magnitude of the point, as if it were a vector
    @return the magnitude of the point
    */
    float mag() const { return sqrtf(x() * x() + y() * y()); }

    /**
    computes magnitude squared
    this is faster than mag()
    @return the magnitude squared
    */
    float magsq() const { return x() * x() + y() * y(); }

    /**
     * @brief Restricts the point to a given magnitude
     * @param max The magnitude to restrict the vector
     */
    Point& clamp(float max) {
        float ratio = mag() / max;
        if (ratio > 1) {
            _x /= ratio;
            _y /= ratio;
        }
        return *this;
    }

    /**
    rotates the point around another point by specified angle in the CCW
    direction
    @param origin the point to rotate around
    @param angle the angle in radians
    */
    Point& rotate(const Point& origin, float angle) {
        *this -= origin;
        rotate(angle);
        *this += origin;
        return *this;
    }

    /**
    * rotates the point around the origin
    */
    Point& rotate(float angle) {
        float newX = x() * cos(angle) - y() * sin(angle);
        float newY = y() * cos(angle) + x() * sin(angle);
        _x = newX;
        _y = newY;
        return *this;
    }

    /**
     * Like rotate(), but returns a new point instead of changing *this
     */
    Point rotated(float angle) const {
        float newX = x() * cos(angle) - y() * sin(angle);
        float newY = y() * cos(angle) + x() * sin(angle);
        return Point(newX, newY);
    }

    /**
     * Returns a new Point rotated around the origin
     */
    Point rotated(const Point& origin, float angle) const {
        return rotated(*this, origin, angle);
    }

    /**
    * static function to use rotate
    */
    static Point rotated(const Point& pt, const Point& origin, float angle) {
        Point newPt = pt;
        newPt.rotate(origin, angle);
        return newPt;
    }

    /**
    computes the distance from the current point to another
    @param other the point to find the distance to
    @return the distance between the points
    */
    float distTo(const Point& other) const {
        Point delta = other - *this;
        return delta.mag();
    }

    /**
    * Returns a vector with the same direction as this vector but with magnitude
    * given,
    * unless this vector is zero.
    * If the vector is (0,0), Point(0,0) is returned
    */
    Point normalized(float magnitude = 1.0) const {
        float m = mag();
        if (m == 0) {
            return Point(0, 0);
        }

        return Point(magnitude * x() / m, magnitude * y() / m);
    }

    /// Alias for normalized() - matches Eigen's syntax
    Point norm() const { return normalized(); }

    /**
    * Returns true if this point is within the given distance (threshold) of
    * (pt)
    */
    bool nearPoint(const Point& other, float threshold) const {
        return (*this - other).magsq() <= (threshold * threshold);
    }

    /**
    * Returns the angle of this point in radians CCW from +X.
    */
    float angle() const { return atan2(y(), x()); }

    /**
    * Returns a unit vector in the given direction (in radians)
    */
    static Point direction(float theta) {
        return Point(cos(theta), sin(theta));
    }

    /** returns the perpendicular to the point, Clockwise */
    Point perpCW() const { return Point(y(), -x()); }

    /** returns the perpendicular to the point, Counter Clockwise */
    Point perpCCW() const { return Point(-y(), x()); }

    /** saturates the magnitude of a vector */
    static Geometry2d::Point saturate(Geometry2d::Point value, float max) {
        float mag = value.mag();
        if (mag > fabs(max)) {
            return value.normalized() * fabs(max);
        }
        return value;
    }

    float angleTo(const Point& other) const { return (other - *this).angle(); }

    float cross(const Point& other) const {
        return x() * other.y() - y() * other.x();
    }

    /** returns the angle between the two normalized points (radians) */
    float angleBetween(const Point& other) const {
        return acos(normalized().dot(other.normalized()));
    }

    bool nearlyEquals(Point other) const;

    std::string toString() const {
        std::stringstream str;
        str << "Point(" << x() << ", " << y() << ")";
        return str.str();
    }

    friend std::ostream& operator<<(std::ostream& stream, const Point& point) {
        stream << point.toString();
        return stream;
    }

private:
    float _x, _y;
};  // \class Point

// global operations

/**
 * adds the * operator for vectors
 * scalar multiplication
 */
inline Point operator*(const float& s, const Point& pt) {
    return Point(pt.x() * s, pt.y() * s);
}
}
