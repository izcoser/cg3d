#include <iostream>
#include "object3d.h"
#include <GL/glut.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

class Pose;

#include <stdlib.h>
#include <string.h>
#include "point.h"

int getNumber(const char* filename){
    int n = 0;
    int first = 1;
    for(int i = 0; filename[i] != '\0'; i++){
        if(filename[i] >= '0' && filename[i] <= '9'){
            if(first){
                n += 10 * filename[i];
                first = 0;
            }
            else{
                n += filename[i];
            }
        }
    }
    return n;
}

int compare(const void* a, const void* b){
    char *const *ia = (char* const*)a;
    char *const *ib = (char* const*)b;
    return getNumber(*ia) > getNumber(*ib);
}

char* concat(const char *s1, const char *s2){
    char *result = (char*)malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

Pose Object3D::loadPose(std::string inputfile){
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./"; // Path to material files
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
    }
        exit(1);  
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    return Pose(reader.GetAttrib(), reader.GetShapes(), reader.GetMaterials());
}

Point getVertexPos(int num, tinyobj::attrib_t attrib){
    tinyobj::real_t vx = attrib.vertices[3*num+0];
    tinyobj::real_t vy = attrib.vertices[3*num+1];
    tinyobj::real_t vz = attrib.vertices[3*num+2];
    return Point(vx, vy, vz);
}

std::vector<Point> Object3D::extractInterestPoints(std::string inputfile){ // obtém pontos de interesse de uma pose.
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./"; // Path to material files
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
    }
        exit(1);  
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    std::vector<Point> points;
    tinyobj::attrib_t attrib = reader.GetAttrib();
    points.push_back(getVertexPos(topOfHeadVertex, attrib));
    points.push_back(getVertexPos(bottomOfHeadVertex, attrib));
    points.push_back(getVertexPos(betweenEyesVertex, attrib));
    points.push_back(getVertexPos(pulseVertex, attrib));
    points.push_back(getVertexPos(elbowVertex, attrib));
    points.push_back(getVertexPos(pulseRightVertex, attrib));
    points.push_back(getVertexPos(rightHandVertex, attrib));
    points.push_back(getVertexPos(leftHandVertex, attrib));
    
    return points;
}

void Object3D::load(const char* decimatedinputdir, GLuint texture, const char* pointsfile, const char* config){
    char * line = NULL; // leitura do arquivo de config.
    size_t len = 0;
    ssize_t read;
    int count = 0;

    FILE* fp = fopen(pointsfile, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        Point topOfHead_;
        Point bottomOfHead_;
        Point betweenEyes_;
        Point pulse_;
        Point elbow_;
        Point pulseRight_;
        Point rightHand_;
        Point leftHand_;

        sscanf(line, "%f,%f,%f;%f,%f,%f;%f,%f,%f;%f,%f,%f;%f,%f,%f;%f,%f,%f;%f,%f,%f;%f,%f,%f\n", &topOfHead_.x, &topOfHead_.y, &topOfHead_.z,
        &bottomOfHead_.x, &bottomOfHead_.y, &bottomOfHead_.z,
        &betweenEyes_.x, &betweenEyes_.y, &betweenEyes_.z,
        &pulse_.x, &pulse_.y, &pulse_.z,
        &elbow_.x, &elbow_.y, &elbow_.z,
        &pulseRight_.x, &pulseRight_.y, &pulseRight_.z,
        &rightHand_.x, &rightHand_.y, &rightHand_.z,
        &leftHand_.x, &leftHand_.y, &leftHand_.z);

        topOfHead.push_back(topOfHead_);
        bottomOfHead.push_back(bottomOfHead_);
        betweenEyes.push_back(betweenEyes_);
        pulse.push_back(pulse_);
        elbow.push_back(elbow_);
        pulseRight.push_back(pulseRight_);
        rightHand.push_back(rightHand_);
        leftHand.push_back(leftHand_);
    }

    fclose(fp);

    fp = fopen(config, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        if(line[0] == '#'){
            continue;
        }
        else{
            switch(count){
                case 0:
                    sscanf(line, "%d,%d\n", &rightPunchFirst, &rightPunchLast);
                    count++;
                    break;
                case 1:
                    sscanf(line, "%d,%d\n", &leftPunchFirst, &leftPunchLast);
                    count++;
                    break;
                case 2:
                    sscanf(line, "%d,%d\n", &walkFirst, &walkLast);
                    count++;
                    break;
                case 3:
                    sscanf(line, "%f\n", &yscale);
                    count++;
                    break;
            }
        }
    }

    fclose(fp);
    free(line);

/*
    count = 0; // leitura da animacao original
    char* filenames[100];
    struct dirent *entry;
    DIR *dir = opendir(inputdir);
    while ((entry = readdir(dir)) != NULL) {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        char* s = concat(inputdir, entry->d_name);
        filenames[count++] = s;
    }
    closedir(dir);

    qsort(filenames, count, sizeof(*filenames), compare);

    for(int i = 0; i < count; i++){
        //printf("extracting points %s\n", filenames[i]);
        std::vector<Point> points = extractInterestPoints(filenames[i]);

        points.at(0).print(';');
        points.at(1).print(';');
        points.at(2).print(';');
        points.at(3).print(';');
        points.at(4).print(';');
        points.at(5).print(';');
        points.at(6).print(';');
        points.at(7).print('\n');

        topOfHead.push_back(points.at(0));
        bottomOfHead.push_back(points.at(1));
        betweenEyes.push_back(points.at(2));
        pulse.push_back(points.at(3));
        elbow.push_back(points.at(4));
        pulseRight.push_back(points.at(5));
        rightHand.push_back(points.at(6));
        leftHand.push_back(points.at(7));
        
        free(filenames[i]);
    }
*/
    count = 0; // leitura de arquivos minimizados
    char* decfilenames[100];
    struct dirent *entry;
    DIR *dir2 = opendir(decimatedinputdir);

    while ((entry = readdir(dir2)) != NULL) {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        char* s = concat(decimatedinputdir, entry->d_name);
        decfilenames[count++] = s;
    }
    closedir(dir2);

    qsort(decfilenames, count, sizeof(*decfilenames), compare);

    for(int i = 0; i < count; i++){
        printf("reading %s\n", decfilenames[i]);
        decimatedPoses.push_back(loadPose(decfilenames[i]));
        free(decfilenames[i]);
    }

    this->texture = texture;
    currentPose = walkFirst;
    punchFrames = rightPunchLast - rightPunchFirst;
    debug = 0;
    target = Point(0, 0, 1);
    theta = 0;
    punchStatus = 0;
    aggressive = 1;
    hitEnabled = 1;
    score = 0;
}

void Object3D::draw(){
    glPushMatrix();

    glScalef(1, yscale, 1);
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(theta, 0, 1, 0);
    
    GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
    GLfloat materialColorA[] = { 0.1, 0.1, 0.1, 0.1};
    GLfloat materialColorD[] = { .90, .90, .90, 1};
    glColor3f(1,1,1);

    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);

    tinyobj::attrib_t attrib = decimatedPoses.at(currentPose).attrib;
    std::vector<tinyobj::shape_t> shapes = decimatedPoses.at(currentPose).shapes;
    std::vector<tinyobj::material_t> materials = decimatedPoses.at(currentPose).materials;
    
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            glBindTexture (GL_TEXTURE_2D, texture);
            glBegin(GL_TRIANGLE_STRIP);
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                
            
                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    glNormal3f((GLfloat)nx, (GLfloat)ny, (GLfloat)nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                    glTexCoord2f (tx, ty);
                }

                glVertex3f((GLfloat)vx, (GLfloat)vy, (GLfloat)vz);

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            glEnd();
            
            index_offset += fv;
            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
    glPopMatrix();

    if(debug){
        glPushMatrix();
        Point headCenter = this->getHeadCenter();
        glTranslatef(headCenter.x, headCenter.y, headCenter.z);
        glutWireSphere(HEAD_RADIUS, 20, 10);
        glPopMatrix();

        glPushMatrix();
        Point rightHand = this->getRightHand();
        glTranslatef(rightHand.x, rightHand.y, rightHand.z);
        glutWireSphere(HAND_RADIUS, 20, 10);
        glPopMatrix();

        glPushMatrix();
        Point leftHand = this->getLeftHand();
        glTranslatef(leftHand.x, leftHand.y, leftHand.z);
        glutWireSphere(HAND_RADIUS, 20, 10);
        glPopMatrix();
    }
}

void Object3D::nextPose(void){
    currentPose += 1;
    currentPose %= decimatedPoses.size();
}

void Object3D::prevPose(void){
    if(currentPose == 0){
        currentPose = decimatedPoses.size() - 1;
    }
    else{
        currentPose -= 1;
    }
}

void Object3D::setPose(GLuint pose){
    this->currentPose = pose;
}

void Object3D::nextWalkingPose(void){
    if(currentPose < walkLast){
        this->nextPose();
    }
    else if(currentPose == walkLast){
        currentPose = walkFirst;
    }
    else if(currentPose < walkFirst){
        currentPose = walkFirst;
    }
}

void Object3D::prevWalkingPose(void){
    if(currentPose > walkFirst){
        this->prevPose();
    }
    else if(currentPose == walkFirst){
        currentPose = walkLast;
    }
    else if(currentPose < walkFirst){
        currentPose = walkLast;
    }
}

void Object3D::toggleDebug(void){
    debug = !debug;
}


Point Object3D::getEyePos(void){
    Point p(0, 0, 0);
    p += betweenEyes.at(currentPose); // reverse transformations
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

Point Object3D::getPulsePos(void){
    Point p(0, 0, 0);
    p += pulse.at(currentPose);
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

Point Object3D::getPulseRightPos(void){
    Point p(0, 0, 0);
    p += pulseRight.at(currentPose);;
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

Point Object3D::getElbowPos(void){
    Point p(0, 0, 0);
    p += elbow.at(currentPose);;
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

Point Object3D::getPulseTarget(void){
    return (this->getPulsePos() - this->getElbowPos()).normalize();
}

Point Object3D::getPulseRightVector(void){
    return (this->getPulseRightPos() - this->getPulsePos()).normalize();
}

Point Object3D::getPulseUp(void){
    return this->getPulseRightVector().cross(this->getPulseTarget());
}

Point Object3D::getHeadTopPos(void){
    Point p(0, 0, 0);
    p += topOfHead.at(currentPose);;
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

Point Object3D::getHeadBottomPos(void){
    Point p(0, 0, 0);
    p += bottomOfHead.at(currentPose);;
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

Point Object3D::getHeadCenter(void){
    return (this->getHeadTopPos() + this->getHeadBottomPos()) / 2;
}

Point Object3D::getLeftHand(void){
    Point p(0, 0, 0);
    p += leftHand.at(currentPose);;
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

Point Object3D::getRightHand(void){
    Point p(0, 0, 0);
    p += rightHand.at(currentPose);;
    p = p.rotatePoint(theta);
    p += pos;
    p = p.scale(Point(1, yscale, 1));
    return p;
}

static int insideArena(Point p, Point arenaDimensions){
    GLfloat radius = BODY_RADIUS;
    return p.x - radius > 0 && p.x + radius < arenaDimensions.x \
    && p.z - radius > 0 && p.z + radius < arenaDimensions.z; 
}

void Object3D::move(GLfloat delta, Object3D computer, GLdouble timeDiff, Point arenaDimensions){
    Point newPos = pos + target * timeDiff * delta;
    GLfloat distance = newPos.distance(computer.pos);
    if(distance > 2 * BODY_RADIUS && insideArena(newPos, arenaDimensions)){
        pos = newPos;
    }
}

void Object3D::rotate(GLfloat delta, GLdouble timeDiff){
    theta += 100 * timeDiff * delta;
    target = Point(0, 0, 1).rotatePoint(theta);
}

int Object3D::hit(Object3D target){
    static GLdouble distance = HAND_RADIUS + HEAD_RADIUS;
    Point headCenter = target.getHeadCenter();
    Point leftHand = this->getLeftHand();
    Point rightHand = this->getRightHand();

    return leftHand.distance(headCenter) < distance || rightHand.distance(headCenter) < distance;
}

void Object3D::lookAt(Object3D target){
    theta = atan2(target.pos.x - pos.x, target.pos.z - pos.z) * 180 / M_PI;
    this->target = Point(0, 0, 1).rotatePoint(theta);
}

void Object3D::punch(GLdouble timeDiff){
    static GLdouble punchTime = 0.0f;
    punchTime += timeDiff;
    
    if(punchTime > TIME_TO_PUNCH){
        punchTime = 0.0f;
        punchStatus = 0;
        this->setPose(walkFirst);
        return;
    }

    if(punchStatus == 1){
        if(punchTime < TIME_TO_PUNCH / 2){
            GLuint pose = round(((GLdouble)punchFrames / (TIME_TO_PUNCH / 2)) * punchTime);
            this->setPose(pose);
        }
        else if(punchTime > TIME_TO_PUNCH / 2){
            GLuint pose = round(((GLdouble)punchFrames / (TIME_TO_PUNCH / 2)) * punchTime * -1) + 2 * punchFrames;
            this->setPose(pose);
        }
    }

    else if(punchStatus == 2){
        if(punchTime < TIME_TO_PUNCH / 2){
            GLuint pose = round(((GLdouble)punchFrames / (TIME_TO_PUNCH / 2)) * punchTime);
            this->setPose(pose + punchFrames + 1);
        }
        else if(punchTime > TIME_TO_PUNCH / 2){
            GLuint pose = round(((GLdouble)punchFrames / (TIME_TO_PUNCH / 2)) * punchTime * -1) + 2 * punchFrames;
            this->setPose(pose + punchFrames + 1);
        }
    }
}

int Object3D::inPunchingDistance(Object3D target){
    return pos.distance(target.pos) < 2.5 * BODY_RADIUS;   
}