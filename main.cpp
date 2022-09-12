#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "point.h"
#include "object3d.h"
#include "imageloader.h"
#include "tinyxml2.h"

#include <list>

#define INC_KEYIDLE 0.01
#define INC_KEY 1
#define min(a,b) ((a) < (b) ? (a) : (b))

/* Global variables */
char title[] = "Trabalho 3D";
int keyStatus[256] = {0};

GLfloat xMouse = 0;
GLfloat yMouse = 0;

Object3D player;
Object3D computer;
GLuint texture; 

int click_x;
int camSwitch = 1;

GLfloat zoom = 2.5;
GLfloat horizontalAngle = 180;
GLfloat verticalAngle = 45;

int leftMouseButtonDown = 0;
int rightMouseButtonDown = 0;

Point arenaDimensions;

int lightToggle = 1;

int skyTexture;
int floorTexture;
int wallTexture;

int enableComputer = 0;
int FPS = 0;

int width = 500;
int height = 700;

int toggleDividedViewPort = 0;

void DrawAxes(double size)
{
    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, 
            no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    //x axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
        glColor3fv(mat_ambient_r);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //y axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
        glColor3fv(mat_ambient_g);
        glRotatef(90,0,0,1);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //z axis
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
        glColor3fv(mat_ambient_b);
        glRotatef(-90,0,1,0);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();
    
}

void RasterChars(GLfloat x, GLfloat y, GLfloat z, const char * text, double r, double g, double b, int font)
{
    //Push to recover original attributes
    glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //Draw text in the x, y, z position
        glColor3f(r,g,b);
        glRasterPos3f(x, y, z);
        const char* tmpStr;
        tmpStr = text;
        if(font == 1){
            while( *tmpStr ){
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *tmpStr);
                tmpStr++;
            }
        }
        else{
            while( *tmpStr ){
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *tmpStr);
                tmpStr++;
            }
        }
        
    glPopAttrib();
}

void PrintText(GLfloat x, GLfloat y, const char * text, double r, double g, double b, int font)
{
    //Draw text considering a 2D space (disable all 3d features)
    glMatrixMode (GL_PROJECTION);
    //Push to recover original PROJECTION MATRIX
    glPushMatrix();
        glLoadIdentity ();
        glOrtho (0, 1, 0, 1, -1, 1);
        RasterChars(x, y, 0, text, r, g, b, font);    
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
}

void CalculateFrameRate(){
    static float framesPerSecond = 0.0f;
    static float lastTime = 0.0f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME);    
    framesPerSecond++;
    if( currentTime - lastTime > 1000.0f )
    {
        lastTime = currentTime;
        //printf("FPS: %d\n", (int)framesPerSecond);
        FPS = (int)framesPerSecond;
        framesPerSecond = 0;
    }
}

void drawSquareTile(double dimension, GLuint texture){
    glBindTexture(GL_TEXTURE_2D, texture);
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(- dimension / 2, 0,  - dimension / 2);
        
        glTexCoord2f(0, 1);
        glVertex3f(- dimension / 2, 0,  dimension / 2);
        
        glTexCoord2f(1, 1);
        glVertex3f(dimension / 2, 0,  dimension / 2);
        
        glTexCoord2f(1, 0);
        glVertex3f(dimension / 2, 0,  - dimension / 2);

    glEnd();
}

void drawArena(double width, double height, double length){
    GLfloat materialEmission[] = { 0.1, 0.1, 0.1, 1};
    GLfloat materialColorA[] = { 0.1, 0.1, 0.1, 1};
    GLfloat materialColorD[] = { 0.9, 0.9, 0.9, 1};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_shininess[] = { 100.0 };
    glColor3f(1,1,1);

    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT  );//X
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );//Y

    double tileDim = 0.5;
    //floor and sky
    int x = (int)(width / tileDim); // x tiles in width
    int y = (int)(length / tileDim); // y tiles in length
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            glPushMatrix();
            glTranslatef(tileDim / 2 + i * tileDim, 0, tileDim / 2 + j * tileDim);
            drawSquareTile(tileDim, floorTexture);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(tileDim / 2 + i * tileDim, height, tileDim / 2 + j * tileDim);
            glRotatef(180, 0, 0, 1);
            drawSquareTile(tileDim, skyTexture);
            glPopMatrix();
        }
    }

    //left and right walls
    x = (int)(width / tileDim); // x tiles in width
    y = (int)(height / tileDim); // y tiles in height
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            glPushMatrix();
            glTranslatef(tileDim / 2 + i * tileDim, tileDim / 2 + j * tileDim, 0);
            glRotatef(90, 1, 0, 0);
            drawSquareTile(tileDim, wallTexture);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(tileDim / 2 + i * tileDim, tileDim / 2 + j * tileDim, length);
            glRotatef(-90, 1, 0, 0);
            drawSquareTile(tileDim, wallTexture);
            glPopMatrix();
        }
    }

    // back and front walls
    x = (int)(length / tileDim); // x tiles in length
    y = (int)(height / tileDim); // y tiles in height
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            glPushMatrix();
            glTranslatef(0, tileDim / 2 + j * tileDim, tileDim / 2 + i * tileDim);
            glRotatef(-90, 0, 0, 1);
            drawSquareTile(tileDim, wallTexture);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(width, tileDim / 2 + j * tileDim, tileDim / 2 + i * tileDim);
            glRotatef(90, 0, 0, 1);
            drawSquareTile(tileDim, wallTexture);
            glPopMatrix();
        }
    }
}

GLuint LoadTextureRAW( const char * filename );

GLfloat radians(GLfloat degrees){
    return degrees * M_PI / 180;
}
 
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat diffuseAndSpecularLight[] = {1, 1, 1, 1};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseAndSpecularLight);
    glLightfv(GL_LIGHT1, GL_SPECULAR, diffuseAndSpecularLight);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseAndSpecularLight);
    glLightfv(GL_LIGHT2, GL_SPECULAR, diffuseAndSpecularLight);
}

void placeLights(){
    if(lightToggle){
        GLfloat lightPosition[] = { arenaDimensions.x / 2, 2, arenaDimensions.z / 2, 1.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    }
    
    else{
        GLfloat playerSpot[] = { player.pos.x, 3, player.pos.z, 1.0 };
        GLfloat spotDir[] = {0, -1, 0};
        glLightfv(GL_LIGHT1, GL_POSITION, playerSpot);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);

        GLfloat computerSpot[] = { computer.pos.x, 3, computer.pos.z, 1.0};
        glLightfv(GL_LIGHT2, GL_POSITION, computerSpot);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir);
    }
}

void display() {
    CalculateFrameRate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    char FPSstring[50];
    snprintf(FPSstring, 12, "FPS: %d", FPS);
    PrintText(0.8, 0.95, FPSstring, 0, 1, 0, 1);

    char Scorestring[50];
    snprintf(Scorestring, 30, "Player %d x %d Computer", player.score, computer.score);
    PrintText(0.20, 0.95, Scorestring, 0, 1, 0, 1);

    if(toggleDividedViewPort){
        glViewport(0, 0, width, 200);
        glLoadIdentity();
        Point cp = computer.getEyePos();
        gluLookAt(cp.x, cp.y, cp.z, computer.target.x + cp.x, computer.target.y + cp.y, computer.target.z + cp.z, 0, 1, 0);
        placeLights();
        player.draw();
        computer.draw();
        drawArena(arenaDimensions.x, arenaDimensions.y, arenaDimensions.z);
        glViewport(0, 200, width, height - 200);
        glLoadIdentity();
    }

    if(computer.score == 10){
        char resultString[100];
        snprintf(resultString, 100, "Voce perdeu do computador por %d x %d!", player.score, computer.score);
        PrintText(0.1, 0.2, resultString, 0, 1, 0, 2);
    }
    else if(player.score == 10){
        char resultString[100];
        snprintf(resultString, 100, "Voce ganhou do computador por %d x %d!", player.score, computer.score);
        PrintText(0.1, 0.2, resultString, 0, 1, 0, 2);
    }

    if(camSwitch == 1){
        Point p = player.getEyePos();
        gluLookAt(p.x, p.y, p.z, player.target.x + p.x, player.target.y + p.y, player.target.z + p.z, 0, 1, 0);
    }

    else if(camSwitch == 2){
        Point p = player.getPulsePos();
        Point target = player.getPulseTarget();
        Point up = player.getPulseUp();
        gluLookAt(p.x, p.y, p.z, target.x + p.x, target.y + p.y, target.z + p.z, up.x, up.y, up.z);
    }

    else if(camSwitch == 3){
        Point target = player.pos + Point(0, 1, 0);
        Point p = target;
        p += Point(zoom * sin(radians(horizontalAngle)) * cos(radians(verticalAngle)),
                    zoom * sin(radians(verticalAngle)),
                    zoom * cos(radians(horizontalAngle)) * cos(radians(verticalAngle)));

        gluLookAt(p.x, p.y, p.z, target.x, target.y, target.z, 0, 1, 0);
    }

    placeLights();

    player.draw();
    computer.draw();
    drawArena(arenaDimensions.x, arenaDimensions.y, arenaDimensions.z);

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
   if (height == 0) height = 1;
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}
 
void keyboard(unsigned char key, int x, int y){
    switch(key){        
        case 'd':
        case 'D':
            keyStatus[(int)('d')] = 1;
            break;
        case 'a':
        case 'A':
            keyStatus[(int)('a')] = 1;
            break;
        case 'w':
        case 'W':
            keyStatus[(int)('w')] = 1;
            break;
        case 's':
        case 'S':
            keyStatus[(int)('s')] = 1;
            break;
        case '1':
            camSwitch = 1;
            break;
        case '2':
            camSwitch = 2;
            break;
        case '3':
            camSwitch = 3;
            break;
        case 'z':
        case 'Z':
            player.toggleDebug();
            computer.toggleDebug();
            break;
        case 'n':
        case 'N':
            lightToggle = !lightToggle;
            if(lightToggle){
                glEnable(GL_LIGHT0);
                glDisable(GL_LIGHT1);
                glDisable(GL_LIGHT2);
            }
            else{
                glDisable(GL_LIGHT0);
                glEnable(GL_LIGHT1);
                glEnable(GL_LIGHT2);
            }
            break;
        
        case 'e':
        case 'E':
            enableComputer = !enableComputer;
            break;

        case 'r':
        case 'R':
            computer.score = player.score = 0;
            player.pos = player.startPos;
            computer.pos = computer.startPos;
            break;

        case 'f':
        case 'F':
            toggleDividedViewPort = !toggleDividedViewPort;
            if(!toggleDividedViewPort){
                glViewport(0, 0, width, height);
            }
            break;
        
        case '+':
            zoom += .1;
            break;

        case '-':
            zoom -= .1;
            break;
        glutPostRedisplay();
    }
}

void keyUp(unsigned char key, int x, int y){
    keyStatus[(int)(key)] = 0;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
    
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        click_x = x;
        leftMouseButtonDown = 1;
    }
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        player.setPose(player.walkFirst);
        leftMouseButtonDown = 0;
    }
    else if(button == GLUT_RIGHT_BUTTON){
        if(state == 0){
            rightMouseButtonDown = 1;
            xMouse = x;
            yMouse = y;
        }
        else{
            rightMouseButtonDown = 0;
        }
    } 

}

void motion(int x, int y){
    if(rightMouseButtonDown){
        horizontalAngle -= x - xMouse;
        verticalAngle += y - yMouse;

        if(verticalAngle > 89.0f){
            verticalAngle = 89.0f;
        }
        if(verticalAngle < -89.0f){
            verticalAngle = -89.0f;
        }

        xMouse = x;
        yMouse = y;
        glutPostRedisplay();
        return;
    }

    else{
        GLfloat punch_percentage = min(1, (GLfloat)(abs(x - click_x)) / ((GLfloat)glutGet(GLUT_WINDOW_WIDTH) / 2));
        GLuint pose = round(punch_percentage * player.punchFrames);
        if(x > click_x){
            /* Right punch*/
            player.setPose(pose);
        }
        else if(x < click_x){
            /* Left punch */
            player.setPose(pose + player.punchFrames + 1);
        }
    }
}

void idle(void){
    if(computer.score == 10 || player.score == 10){
        return;
    }

    static GLdouble previous_time = glutGet(GLUT_ELAPSED_TIME);
    GLdouble current_time, timeDiff;
    static GLdouble computer_timer = 0;
    current_time = glutGet(GLUT_ELAPSED_TIME);
    timeDiff = current_time - previous_time;
    previous_time = current_time;
    computer_timer += timeDiff;

    GLfloat inc = INC_KEYIDLE / 5;

    computer.lookAt(player);

    if(keyStatus[(int)('w')]){
        player.move(inc, computer, timeDiff, arenaDimensions);
        player.nextWalkingPose();
    }
    if(keyStatus[(int)('s')]){
        player.move(-inc, computer, timeDiff, arenaDimensions);
        player.prevWalkingPose();
    }
    if(keyStatus[(int)('a')]){
        player.rotate(inc, timeDiff);
    }
    if(keyStatus[(int)('d')]){
        player.rotate(-inc, timeDiff);
    }

    if(enableComputer){
        if(computer.aggressive){
            computer.move(inc, player, timeDiff, arenaDimensions);
            computer.nextWalkingPose();
            if(computer.punchStatus == 0 && computer.inPunchingDistance(player)){
                if(rand() % 2){
                    computer.punchStatus = 1; // soco direito
                }
                else{
                    computer.punchStatus = 2; // soco esquerdo
                }
            }
        }
        else{
            computer.move(-inc, player, timeDiff, arenaDimensions);
            computer.prevWalkingPose();
        }
    }

    if(computer.punchStatus){
        computer.punch(timeDiff);
    }

    if(computer.punchStatus && computer.hit(player)){
        if(computer.hitEnabled){
            computer.score += 1;
            computer.hitEnabled = 0;
        }
    }
    else{
        computer.hitEnabled = 1;
    }

    if(leftMouseButtonDown && player.hit(computer)){
        if(player.hitEnabled){
            player.score += 1;
            player.hitEnabled = 0;
        }
    }
    else{
        player.hitEnabled = 1;
    }

    if(computer_timer > 5000){
        computer_timer = 0;
        computer.aggressive = !computer.aggressive;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    srand(time(NULL));

    tinyxml2::XMLDocument doc;
    doc.LoadFile(argv[1]);
    tinyxml2::XMLElement* rect = doc.FirstChildElement("svg")->FirstChildElement("rect");
    int rect_x = atoi(rect->Attribute("x"));
    int rect_y = atoi(rect->Attribute("y"));

    arenaDimensions.x = (GLfloat)atof(rect->Attribute("width")) / 50;
    arenaDimensions.z = (GLfloat)atof(rect->Attribute("height")) / 50;
    arenaDimensions.y = 3;

    tinyxml2::XMLElement* circle_1 = doc.FirstChildElement("svg")->FirstChildElement("circle");
    int circle_1_x = (atoi(circle_1->Attribute("cx")) - rect_x) /50;
    int circle_1_y = (atoi(circle_1->Attribute("cy")) - rect_y) /50;
    int circle_1_r = atoi(circle_1->Attribute("r"));
    const char* circle_1_fill = circle_1->Attribute("fill");

    tinyxml2::XMLElement* circle_2 = doc.FirstChildElement("svg")->LastChildElement("circle");
    int circle_2_x = (atoi(circle_2->Attribute("cx")) - rect_x) /50;
    int circle_2_y = (atoi(circle_2->Attribute("cy")) - rect_y) /50;
    int circle_2_r = atoi(circle_2->Attribute("r"));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);

    if(argc > 2 && strcmp(argv[2], "full") == 0){
        width = 1280;
        height = 720;
    }

    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);
    glutCreateWindow(title);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyUp);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);
    initGL();
    player.load("./models/amy/decimated/", LoadTextureRAW("./models/amy/amy.bmp"), "./models/amy/amy.points", "./models/amy/amy.newconfig");
    computer.load("./models/michelle/decimated/", LoadTextureRAW("./models/michelle/michelle.bmp"), "./models/michelle/michelle.points", "./models/michelle/michelle.newconfig");

    if(strcmp(circle_1_fill, "green") == 0){ // O jogador e sempre o verde.
        player.pos = player.startPos = Point(circle_1_x, 0, circle_1_y);
        computer.pos = computer.startPos = Point(circle_2_x, 0, circle_2_y);
    }
    else{
        player.pos = player.startPos = Point(circle_2_x, 0, circle_2_y);
        computer.pos = computer.startPos = Point(circle_1_x, 0, circle_1_y);
    }

    player.lookAt(computer);
    skyTexture = LoadTextureRAW("./roof.bmp");
    floorTexture = LoadTextureRAW("./trakpanel.bmp");
    wallTexture = LoadTextureRAW("./wood.bmp");

    printf("Iniciando jogo: arena -> width: %.2f, height %.2f, length: %.2f\n", arenaDimensions.x, arenaDimensions.y, arenaDimensions.z);
    printf("Player pos: ");
    player.pos.print(';');
    printf("Computer pos: ");
    computer.pos.print('\n');

    glutMainLoop();
    
    return 0;
}

GLuint LoadTextureRAW( const char * filename ){

    GLuint texture;
    
    Image* image = loadBMP(filename);

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                             0,                            //0 for now
                             GL_RGB,                       //Format OpenGL uses for image
                             image->width, image->height,  //Width and height
                             0,                            //The border of the image
                             GL_RGB, //GL_RGB, because pixels are stored in RGB format
                             GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                                               //as unsigned numbers
                             image->pixels);               //The actual pixel data
    delete image;

    return texture;
}
