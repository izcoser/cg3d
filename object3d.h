#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <GL/gl.h>
#include <GL/glu.h>
#include "tiny_obj_loader.h"
#include "point.h"

class Pose{
    public:
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        Pose(tinyobj::attrib_t attrib, std::vector<tinyobj::shape_t> shapes, std::vector<tinyobj::material_t> materials){
            this->attrib = attrib;
            this->shapes = shapes;
            this->materials = materials;
        }
};

class Object3D{
    public:
        std::vector<Pose> poses;
        GLuint texture;
        GLuint currentPose;

        int debug;

        Point pos;
        Point target; // lookat
        GLfloat theta; // rotation y axis

        int rightPunchFirst;
        int rightPunchLast;

        int leftPunchFirst;
        int leftPunchLast;

        int walkFirst;
        int walkLast;

        int topOfHeadVertex;
        int bottomOfHeadVertex;
        int betweenEyesVertex;
        int centerVertex;
        int pulseVertex;
        int pulseRightVertex;
        int elbowVertex;

        int rightHandVertex;
        int leftHandVertex;

        int punchFrames;

        Pose loadPose(std::string inputfile);
        void load(const char* inputdir, GLuint texture, const char* config);
        void draw(void);
        void nextPose(void);
        void prevPose(void);
        void setPose(GLuint pose);
        void nextWalkingPose(void);
        void prevWalkingPose(void);
        void toggleDebug(void);
        Point getVertexPos(int num);
        Point getEyePos(void);
        Point getPulsePos(void);
        Point getPulseRightPos(void);
        Point getElbowPos(void);
        Point getPulseTarget(void);
        Point getPulseUp(void);
        Point getPulseRightVector(void);
        Point getCenterPos(void);
        Point getHeadTopPos(void);
        Point getHeadBottomPos(void);
        Point getHeadCenter(void);
        Point getLeftHand(void);
        Point getRightHand(void);
};

#endif