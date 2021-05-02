#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "point.h"
#include "object3d.h"
#include "imageloader.h"

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

GLfloat zoom = 3;
GLfloat horizontalAngle = 180;
GLfloat verticalAngle = 45;

int leftMouseButtonDown = 0;
int rightMouseButtonDown = 0;

Point arenaDimensions(10, 3, 10);

int lightToggle = 1;

void CalculateFrameRate(){
    static float framesPerSecond = 0.0f;
    static float lastTime = 0.0f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME);    
    framesPerSecond++;
    if( currentTime - lastTime > 1000.0f )
    {
        lastTime = currentTime;
        printf("FPS: %d\n", (int)framesPerSecond);
        framesPerSecond = 0;
    }
}


void drawArena(double width, double height, double length){
    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat mat_ambient_c[] = { 0.0, 0.5, 0.5, 1.0 }; // cyan
    GLfloat mat_ambient_y[] = { 0.5, 0.5, 0.0, 1.0 }; // yellow
    GLfloat mat_ambient_p[] = { 0.5, 0.0, 0.5, 1.0 }; // purple

    glBegin(GL_QUADS);
    
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
    glColor3fv(mat_ambient_r);
    
    glNormal3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glNormal3f(1, 0, 0);
    glVertex3f(0, 0, length);
    glNormal3f(1, 0, 0);
    glVertex3f(0, height, length);
    glNormal3f(1, 0, 0);
    glVertex3f(0, height, 0);


    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
    glColor3fv(mat_ambient_g);

    glNormal3f(0, 0, -1);
    glVertex3f(0, 0, length);
    glNormal3f(0, 0, -1);
    glVertex3f(width, 0, length);
    glNormal3f(0, 0, -1);
    glVertex3f(width, height, length);
    glNormal3f(0, 0, -1);
    glVertex3f(0, height, length);

    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
    glColor3fv(mat_ambient_b);

    glNormal3f(-1, 0, 0);
    glVertex3f(width, 0, 0);
    glNormal3f(-1, 0, 0);
    glVertex3f(width, 0, length);
    glNormal3f(-1, 0, 0);
    glVertex3f(width, height, length);
    glNormal3f(-1, 0, 0);
    glVertex3f(width, height, 0);

    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_c);
    glColor3fv(mat_ambient_c);

    glNormal3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glNormal3f(0, 0, 1);
    glVertex3f(width, 0, 0);
    glNormal3f(0, 0, 1);
    glVertex3f(width, height, 0);
    glNormal3f(0, 0, 1);
    glVertex3f(0, height, 0);

    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_y);
    glColor3fv(mat_ambient_y);

    glNormal3f(0, -1, 0);
    glVertex3f(0, height, 0);
    glNormal3f(0, -1, 0);
    glVertex3f(width, height, 0);
    glNormal3f(0, -1, 0);
    glVertex3f(width, height, length);
    glNormal3f(0, -1, 0);
    glVertex3f(0, height, length);

    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_p);
    glColor3fv(mat_ambient_p);

    glNormal3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glNormal3f(0, 1, 0);
    glVertex3f(width, 0, 0);
    glNormal3f(0, 1, 0);
    glVertex3f(width, 0, length);
    glNormal3f(0, 1, 0);
    glVertex3f(0, 0, length);

    glEnd();
}

void DrawAxes(double size){
    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, 
            no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    //x axis red
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, 
                mat_ambient_r);
        glColor3fv(mat_ambient_r);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //y axis green
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, 
                mat_ambient_g);
        glColor3fv(mat_ambient_g);
        glRotatef(90,0,0,1);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //z axis blue
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
        glColor3fv(mat_ambient_b);
        glRotatef(-90,0,1,0);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();    
}

GLuint LoadTextureRAW( const char * filename );

GLfloat radians(GLfloat degrees){
    return degrees * M_PI / 180;
}
 
/* Initialize OpenGL Graphics */
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    //glEnable(GL_CULL_FACE); //?
    //glShadeModel(GL_SMOOTH);   // Enable smooth shading
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections

    // Create light components.
    //GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    //GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
    //GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    //GLfloat position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //GLfloat position1[] = { 0.0f, 0.0f, -10.0f, 1.0f };

    // Assign created components to GL_LIGHT0.
    //glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    //glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void drawObj(double size){   
    GLfloat materialEmission[] = { 0.00, 0.00, 0.00, 1.0};
    GLfloat materialColor[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = { 128 };
    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glColor3f(1,0,0);
    glutSolidCube(size);
    //glutSolidSphere(size, 20, 10);
}

void drawFloor(){
    GLfloat materialEmission[] = { 0.00, 0.00, 0.00, 1.0};
    GLfloat materialColor[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = { 128 };
    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glBegin(GL_LINES);
        for(int i = -100; i < 100; i += 4){
            glVertex3f(i, 0, 0);
            glVertex3f(i, 0, 100);

            glVertex3f(-100, 0, i);
            glVertex3f(0, 0, i);
        }
    glEnd();
}

void display() {
    CalculateFrameRate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    
    // Render a color-cube consisting of 6 quads with different colors
    glLoadIdentity();                 // Reset the model-view matrix

    
    if(lightToggle){
        GLfloat light_position[] = { arenaDimensions.x / 2, arenaDimensions.y, arenaDimensions.z / 2, 1.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    }
    
    else{
        Point spot1 = player.pos;
        spot1.y = arenaDimensions.y;

        Point spot2 = computer.pos;
        spot2.y = arenaDimensions.y;

        GLfloat spot1_pos[] = {spot1.x, spot1.y, spot1.z};
        GLfloat spot1_dir[] = {0, -1, 0};
        glLightfv(GL_LIGHT1, GL_POSITION, spot1_pos);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 5.0);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot1_dir);

        GLfloat spot2_pos[] = {spot2.x, spot2.y, spot2.z};
        GLfloat spot2_dir[] = {0, -1, 0};
        glLightfv(GL_LIGHT2, GL_POSITION, spot2_pos);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 5.0);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot2_dir);
    }
    
    if(camSwitch == 1){
        Point p = player.getEyePos();
        gluLookAt(p.x, p.y, p.z, player.target.x + p.x, player.target.y + p.y, player.target.z + p.z, 0, 1, 0);
    }

    else if(camSwitch == 2){
        /*Point p = player.getPulsePos();
        Point target = player.getPulseTarget();
        gluLookAt(p.x, p.y, p.z, target.x + p.x, target.y + p.y, target.z + p.z, 0, 1, 0); se up puder ser constante 0, 1, 0 
        */
        Point p = player.getPulsePos();
        Point target = player.getPulseTarget();
        Point up = player.getPulseUp();
        gluLookAt(p.x, p.y, p.z, target.x + p.x, target.y + p.y, target.z + p.z, up.x, up.y, up.z);
    }

    else if(camSwitch == 3){
        /*
        Point p = player.pos - (player.target * thirdPersonZoom) + Point(0, 2, 0);
        //Point target = player.getCenterPos(); // fica balançando pois o centro do jogador varia ao andar.
        Point target = player.pos + Point(0, 1, 0);
        gluLookAt(p.x, p.y, p.z, target.x, target.y, target.z, 0, 1, 0);*/

        Point target = player.pos + Point(0, 1, 0);
        Point p = target;
        p += Point(zoom * sin(radians(horizontalAngle)) * cos(radians(verticalAngle)),
                    zoom * sin(radians(verticalAngle)),
                    zoom * cos(radians(horizontalAngle)) * cos(radians(verticalAngle)));

        gluLookAt(p.x, p.y, p.z, target.x, target.y, target.z, 0, 1, 0);
    }
    
    /*drawFloor();
    DrawAxes(1.5);
*/
    player.draw();
    computer.draw();

    drawArena(arenaDimensions.x, arenaDimensions.y, arenaDimensions.z);

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
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
        if(verticalAngle < 1.0f){
            verticalAngle = 1.0f;
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
    static GLdouble previous_time = glutGet(GLUT_ELAPSED_TIME);
    GLdouble current_time, timeDiff;
    current_time = glutGet(GLUT_ELAPSED_TIME);
    timeDiff = current_time - previous_time;
    previous_time = current_time;

    GLfloat inc = INC_KEYIDLE / 5;

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

    if(leftMouseButtonDown && player.hit(computer)){ // curto circuito pra não ficar calculando
        printf("Hit!\n");                            //  hit o tempo inteiro
    }

    glutPostRedisplay();
}


/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
    glutInit(&argc, argv);            // Initialize GLUT
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(1280, 720);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutCreateWindow(title);          // Create window with the given title
    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);       // Register callback handler for window re-size event
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyUp);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);
    initGL();
    player.load("./models/amy/animations/", LoadTextureRAW("./models/amy/amy.bmp"), "./models/amy/amy.config");
    player.pos = Point(1, 0, 1);


    computer.load("./models/michelle/animations/", LoadTextureRAW("./models/michelle/michelle.bmp"), "./models/michelle/michelle.config");
    computer.pos = Point(8, 0, 8);
    computer.toggleDebug();
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