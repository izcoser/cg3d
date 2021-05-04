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
        /* Listas de pontos de interesse.
        Para cada mesh da animação original, são escolhidos pontos como cabeça, meio dos olhos, mãos.
        Esses pontos serão usados para fazer cálculos de colisão e posicionamento de câmeras. 
        Essa informação vem do arquivo de configuração. */

        std::vector<Point> topOfHead;
        std::vector<Point> bottomOfHead;
        std::vector<Point> betweenEyes;
        std::vector<Point> pulse;
        std::vector<Point> elbow;
        std::vector<Point> pulseRight;
        std::vector<Point> rightHand;
        std::vector<Point> leftHand;

        
        /* Somente as poses decimated (10% do original) são armazenadas.
        Os pontos dessas poses serão usados apenas para desenho, enquanto os pontos originais (acima) 
        serão usados para cálculos. */
        std::vector<Pose> decimatedPoses;
        
        
        GLuint texture;
        GLuint currentPose;

        int debug;

        Point pos;
        Point startPos;
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
        int pulseVertex;
        int pulseRightVertex;
        int elbowVertex;

        int rightHandVertex;
        int leftHandVertex;

        int punchFrames;

        GLfloat yscale;

        int hitEnabled;
        int score;

        // Atributos abaixo usados apenas para o computador. 
        int punchStatus; // 1: soco com mão direita
                         // 2: soco com mão esquerda.

        int aggressive;

        Pose loadPose(std::string inputfile);
        std::vector<Point> extractInterestPoints(std::string inputfile);
        void load(const char* decimatedinputdir, GLuint texture, const char* pointsfile, const char* config);
        void draw(void);
        void nextPose(void);
        void prevPose(void);
        void setPose(GLuint pose);
        void nextWalkingPose(void);
        void prevWalkingPose(void);
        void toggleDebug(void);
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