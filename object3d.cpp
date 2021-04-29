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

void Object3D::load(const char* inputdir, GLuint texture, const char* config, int animated){
    if(animated){
        int count = 0;
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
            printf("reading %s\n", filenames[i]);
            poses.push_back(loadPose(filenames[i]));
            free(filenames[i]);
        }

        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        count = 0;

        FILE* fp = fopen(config, "r");
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
                        sscanf(line, "%d,%d\n", &topOfHeadVertex, &bottomOfHeadVertex);
                        count++;
                        break;
                    case 4:
                        sscanf(line, "%d", &betweenEyesVertex);
                        count++;
                        break;
                    case 5:
                        sscanf(line, "%d", &centerVertex);
                        count++;
                        break;
                    case 6:
                        sscanf(line, "%d,%d,%d\n", &pulseVertex, &elbowVertex, &pulseRightVertex);
                        count++;
                        break;
                    case 7:
                        sscanf(line, "%d,%d,%d\n", &rightHandVertex, &leftHandVertex);
                        count++;
                        break;
                }
            }
        }

        fclose(fp);
        if (line){
            free(line);
        }

        this->texture = texture;
        this->currentPose = walkFirst;
        this->punchFrames = this->rightPunchLast - this->rightPunchFirst;
        debug = 0;
        pos = Point(0, 0, 0);
        target = Point(0, 0, 1);
        theta = 0;
        }

    else{
        struct dirent *entry;
        DIR *dir = opendir(inputdir);
        while ((entry = readdir(dir)) != NULL) {
            if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;

            char* s = concat(inputdir, entry->d_name);
            poses.push_back(loadPose(s));
        }
        closedir(dir);
        this->texture = texture;
        this->currentPose = 0;
        pos = Point(0, 0, 20);
    }

}

void Object3D::draw(){
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(theta, 0, 1, 0);

    GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
    GLfloat materialColorA[] = { 0.1, 0.1, 0.1, 0.1};
    GLfloat materialColorD[] = { .90, .90, .90, 1};
    glColor3f(1,1,1);

    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);

    tinyobj::attrib_t attrib = poses.at(currentPose).attrib;
    std::vector<tinyobj::shape_t> shapes = poses.at(currentPose).shapes;
    std::vector<tinyobj::material_t> materials = poses.at(currentPose).materials;
    
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
        glutWireSphere(0.12, 20, 10);
        glPopMatrix();

        glPushMatrix();
        Point rightHand = this->getRightHand();
        glTranslatef(rightHand.x, rightHand.y, rightHand.z);
        glutWireSphere(0.06, 20, 10);
        glPopMatrix();

        glPushMatrix();
        Point leftHand = this->getLeftHand();
        glTranslatef(leftHand.x, leftHand.y, leftHand.z);
        glutWireSphere(0.06, 20, 10);
        glPopMatrix();
    }
}

void Object3D::nextPose(void){
    currentPose += 1;
    currentPose %= poses.size();
}

void Object3D::prevPose(void){
    if(currentPose == 0){
        currentPose = poses.size() - 1;
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

Point Object3D::getVertexPos(int num){
    tinyobj::attrib_t attrib = poses.at(currentPose).attrib;
    tinyobj::real_t vx = attrib.vertices[3*num+0];
    tinyobj::real_t vy = attrib.vertices[3*num+1];
    tinyobj::real_t vz = attrib.vertices[3*num+2];
    return Point(vx, vy, vz);
}

Point Object3D::getEyePos(void){
    Point p(0, 0, 0);
    p += getVertexPos(betweenEyesVertex); // reverse transformations
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}

Point Object3D::getPulsePos(void){
    Point p(0, 0, 0);
    p += getVertexPos(pulseVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}

Point Object3D::getPulseRightPos(void){
    Point p(0, 0, 0);
    p += getVertexPos(pulseRightVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}

Point Object3D::getElbowPos(void){
    Point p(0, 0, 0);
    p += getVertexPos(elbowVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}

Point Object3D::getPulseTarget(void){
    return (this->getPulsePos() - this->getElbowPos()).normalize();
}

Point Object3D::getCenterPos(void){
    Point p(0, 0, 0);
    p += getVertexPos(centerVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;    
}

Point Object3D::getPulseRightVector(void){
    return (this->getPulseRightPos() - this->getPulsePos()).normalize();
}

Point Object3D::getPulseUp(void){
    return this->getPulseRightVector().cross(this->getPulseTarget());
}

Point Object3D::getHeadTopPos(void){
    Point p(0, 0, 0);
    p += getVertexPos(topOfHeadVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}

Point Object3D::getHeadBottomPos(void){
    Point p(0, 0, 0);
    p += getVertexPos(bottomOfHeadVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}

Point Object3D::getHeadCenter(void){
    return (this->getHeadTopPos() + this->getHeadBottomPos()) / 2;
}

Point Object3D::getLeftHand(void){
    Point p(0, 0, 0);
    p += getVertexPos(leftHandVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}

Point Object3D::getRightHand(void){
    Point p(0, 0, 0);
    p += getVertexPos(rightHandVertex);
    p = p.rotatePoint(theta);
    p += pos;
    return p;
}