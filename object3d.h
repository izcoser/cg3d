#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <GL/gl.h>
#include <GL/glu.h>
#include "tiny_obj_loader.h"
#include "point.h"

#define BODY_RADIUS 0.4
#define HAND_RADIUS 0.08
#define HEAD_RADIUS 0.16
#define TIME_TO_PUNCH 500.0f
#define minimo(a,b) ((a) < (b) ? (a) : (b))

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

        GLfloat yscale;

        // Atributos abaixo usados apenas para o computador. 
        int punchStatus; // 1: soco com mão direita
                         // 2: soco com mão esquerda.

        int aggressive;

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
        void move(GLfloat delta, Object3D computer, GLdouble timeDiff, Point arenaDimensions);
        void rotate(GLfloat delta, GLdouble timeDiff);
        int hit(Object3D target);
        void lookAt(Object3D target);
        void punch(GLdouble currentTime);
        int inPunchingDistance(Object3D target);
};

#endif