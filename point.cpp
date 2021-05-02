#include "point.h"
#include "math.h"

Point::Point(GLfloat _x, GLfloat _y, GLfloat _z){
    x = _x;
    y = _y;
    z = _z;
}

GLfloat Point::distance(Point a){
    return sqrt(pow(x - a.x, 2) + pow(y - a.y, 2) + pow(z - a.z, 2)); 
}

Point Point::sum(Point a){
    return Point(x + a.x, y + a.y, z + a.z);
}

Point Point::normalize(void){
    GLfloat length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    return Point(x / length, y / length, z / length);
}

Point Point::cross(Point b){
    return Point(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);   
}

Point Point::operator+(const Point& b){
    return Point(x + b.x, y + b.y, z + b.z);
}

Point& Point::operator+=(const Point& b){
    x += b.x;
    y += b.y;
    z += b.z;
    return *this;
}

Point Point::operator-(const Point& b){
    return Point(x - b.x, y - b.y, z - b.z);
}

Point& Point::operator-=(const Point& b){
    x -= b.x;
    y -= b.y;
    z -= b.z;
    return *this;
}

Point Point::operator*(const GLdouble& t){
    return Point(x * t, y * t, z * t);
}

Point Point::operator/(float t){
    return Point(x / t, y / t, z / t);
}

int Point::isClose(Point b, GLfloat delta){
    return this->distance(b) <= delta && x != b.x && z != b.z;
}

// Rotates with an angle around Y, theta in degrees.
Point Point::rotatePoint(GLfloat theta){
    theta = theta * M_PI / 180;
    GLfloat xx = x;
    GLfloat zz = z;
    return Point(xx * cos(theta) + zz * sin(theta), y, -xx * sin(theta) + zz * cos(theta));
}

Point Point::scale(Point s){
    return Point(x * s.x, y * s.y, z * s.z);
}

/*
//Funcao auxiliar para fazer o produto vetorial entre dois vetores a x b = out
void cross(float a[3], float b[3], float out[3])
{
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

*/
/*Point Point::rotate(GLfloat angle){
    angle *= M_PI / 180;
    GLfloat s = sin(angle);
    GLfloat c = cos(angle);
    return Point(x * c - y * s, x * s + y * c);
}*/