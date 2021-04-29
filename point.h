#ifndef POINT_H
#define POINT_H

#include <GL/gl.h>
#include <GL/glu.h>

class Point{
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        
        GLfloat distance(Point a);

        Point(GLfloat _x, GLfloat _y, GLfloat _z);
        Point() = default;
        Point sum(Point a);
        Point normalize(void);
        Point cross(Point b);

        Point operator+(const Point& b);
        Point& operator+=(const Point& b);
        Point operator-(const Point& b);
        Point& operator-=(const Point& b);
        Point operator*(const GLdouble& t);
        Point operator/(float t);
        int isClose(Point b, GLfloat delta);
        Point rotatePoint(GLfloat theta);

        //Point rotate(GLfloat angle); /* Angle in degrees. */
};

#endif