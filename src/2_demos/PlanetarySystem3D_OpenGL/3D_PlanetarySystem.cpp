/*

 3D Planetary System in OpenGL

 Filename = 3D_PlanetarySystem.cpp

 Created by Pritam Zope

*/

#include "stdafx.h"
#include<Windows.h>
#include<gl/glut.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<conio.h>
#include<stdio.h>
#include<math.h>
#include<string.h>

static float sun = 0.0;
int sec = 10;
static float planet = 0.0;

float _Angle = 0.0;
float _moveLeftRight = 0.0;
float X = -5.0, Z = 150.0;
float X_2 = 0.0, Z_2 = -1.0;
float Y = 3.0;
int _moveForeBack = 0;
int moveUp = 0;
int _width, _height;
float fb = 0.8;

//function to read image file & bind that image file with textures 2D
GLuint LoadTextureImageFile(const char * filename)
{
    GLuint texture = 0;
    int width, height;
    BYTE * data = NULL;
    FILE * file;

    // open texture data
    fopen_s(&file, filename, "rb");

    if (&file == NULL) return 0;

    // allocate buffer
    width = 256;
    height = 256;

    data = (BYTE*)malloc(width * height * 3);

    // read texture data
    fread(data, width * height * 3, 1, file);
    fclose(file);

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    //  texture MIP maps
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_BGRA_EXT, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);


    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_COLOR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // free buffer
    free(data);

    return texture;

}


//delete data from created texture
void FreeCreatedTexture(GLuint texture)
{
    glDeleteTextures(1, &texture);
}


//initialize opengl
void Init_OpenGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
}


//draw space means stars with texture stars
void Draw_Space()
{
    GLuint texture = LoadTextureImageFile("textures/stars.bmp");

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    //top
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-500.0, 300.0, -500.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(500.0, 300.0, -500.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(500.0, 300.0, 500.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-500.0, 300.0, 500.0);
    glEnd();

    //front
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-500.0, 300.0, -500.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(500.0, 300.0, -500.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(500.0, -300.0, -500.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-500.0, -300.0, -500.0);
    glEnd();

    //left
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-500.0, 300.0, -500.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-500.0, 300.0, 500.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-500.0, -300.0, 500.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-500.0, -300.0, -500.0);
    glEnd();

    //right
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(500.0, 300.0, -500.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(500.0, 300.0, 500.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(500.0, -300.0, 500.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(500.0, -300.0, -500.0);
    glEnd();

    //back
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-500.0, 300.0, 500.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(500.0, 300.0, 500.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(500.0, -300.0, 500.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-500.0, -300.0, 500.0);
    glEnd();

    //bottom
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-500.0, -300.0, -500.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(500.0, -300.0, -500.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(500.0, -300.0, 500.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-500.0, -300.0, 500.0);
    glEnd();

}


//draw all planets using gluSphere() method & also setting textures
void Draw_Planets()
{

    GLuint texture;

    GLUquadric *quad;


    //////////////////////////////////////////////////////////////////////////
    //draw mercury

    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/mercury.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 10);
    glTranslatef(-55.0, 0.0, 0.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 4.0, 60, 60);

    FreeCreatedTexture(texture);

    glPopMatrix();


    //////////////////////////////////////////////////////////////////
    //draw venus
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/venus.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 50);
    glTranslatef(75.0, 0.0, 30.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 4.5, 40, 40);

    FreeCreatedTexture(texture);

    glPopMatrix();


    /////////////////////////////////////////////////////////
    //draw earth
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/earth.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 1);
    glTranslatef(0.0, 0.0,  120.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 5.0, 70, 70);

    FreeCreatedTexture(texture);

    glPopMatrix();

    //draw earth's moon
    glPushMatrix();

    texture = LoadTextureImageFile("textures/moon.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 1);
    glTranslatef(5.0, 0.0, 133.0);
    gluSphere(quad, 1.5, 40, 40);

    FreeCreatedTexture(texture);

    glPopMatrix();


    //////////////////////////////////////////////////////////////
    //draw mars
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/mars.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 10);
    glTranslatef(-60.0, 0.5, 180.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 4.0, 40, 40);

    FreeCreatedTexture(texture);

    glPopMatrix();


    //////////////////////////////////////////////////////////////////
    //draw jupiter
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/jupiter.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 10);
    glTranslatef(-100.0, 0.0, -280.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 20.0, 100, 100);

    FreeCreatedTexture(texture);

    glPopMatrix();


    ///////////////////////////////////////////////////////////////
    //draw saturn
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/saturn.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 1);
    glTranslatef(300.0, 2.0, -360.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 12.0, 100, 100);
    glRotatef(90, 1.0, 0.0, 0.0);
    gluDisk(quad, 20.0, 28.0, 80, 80);

    FreeCreatedTexture(texture);

    glPopMatrix();


    /////////////////////////////////////////////////////////////
    //draw uranus
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/uranus.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 1);
    glTranslatef(50.0, 1.0, -360.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 8.0, 100, 100);

    FreeCreatedTexture(texture);

    glPopMatrix();


    /////////////////////////////////////////////////////////
    //draw neptune
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/neptune.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 50);
    glTranslatef(250.0, 1.0, 100.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 8.0, 100, 100);

    FreeCreatedTexture(texture);

    glPopMatrix();


    //////////////////////////////////////////////////
    //draw pluto
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);

    texture = LoadTextureImageFile("textures/pluto.bmp");
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 5);
    glTranslatef(-250.0, 1.0, 100.0);
    glRotatef((GLfloat)planet, 0.0, 1.0, 0.0);
    gluSphere(quad, 8.0, 100, 100);

    FreeCreatedTexture(texture);

    glPopMatrix();

}

//function that moves you to left & right direction
void Moving_Left_Right_Direction(float angle)
{
    X_2 = sin(angle);
    Z_2 = -cos(angle);
    glLoadIdentity();
    gluLookAt(X, Y, Z, X + X_2, Y, Z + Z_2, 0.0f, 1.0f, 0.0f);
}

//function that moves you to forward & backward direction
void Moving_Foreword_Backword_Direction(int loc)
{
    X = X + loc*(X_2)*fb;
    Z = Z + loc*(Z_2)*fb;
    glLoadIdentity();
    gluLookAt(X, Y, Z, X + X_2, Y, Z + Z_2, 0.0f, 1.0f, 0.0f);
}


//display scene
void Display_Scene()
{

    if (_moveForeBack)
    {
        Moving_Foreword_Backword_Direction(_moveForeBack);
    }

    if (_moveLeftRight)
    {
        _Angle += _moveLeftRight;
        Moving_Left_Right_Direction(_Angle);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint texture = LoadTextureImageFile("textures/sun.bmp");

    GLUquadric *quad;

    glPushMatrix();

    //Draw_Space();

    ///////////////////////////////////////////////////////////
    //draw sun
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    quad = gluNewQuadric();
    gluQuadricTexture(quad, 40);

    glTranslatef(0.0, 0.0, 0.0);

    //rotating sun as well as all planets to Y axis
    glRotatef((GLfloat)sun, 0.0, 1.0, 0.0);
    gluSphere(quad, 30.0, 100, 100);

    FreeCreatedTexture(texture);

    Draw_Planets();

    glPopMatrix();

    glutSwapBuffers();

    sun += 0.04f;
    planet += 0.5f;
}


//keyboard key function
void Keyboard_function(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        exit(EXIT_SUCCESS);
    }
}

//keyboard key pressed function
void Key_Pressed(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
    {
        _moveLeftRight = -0.09;
    }
    else if (key == GLUT_KEY_RIGHT)
    {
        _moveLeftRight = 0.09;
    }
    else if (key == GLUT_KEY_UP)
    {
        _moveForeBack = 8;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        _moveForeBack = -8;
    }
    else if (key == GLUT_KEY_UP && key == GLUT_KEY_LEFT)
    {
        _moveLeftRight = 0.07;
    }
    else if (key == GLUT_KEY_UP && key == GLUT_KEY_RIGHT)
    {
        _moveLeftRight = 0.07;
    }
    else if (key == GLUT_KEY_PAGE_UP)
    {
        Y += 2.0;
        glLoadIdentity();
        gluLookAt(X, Y, Z, X + X_2, Y, Z + Z_2, 0.0f, 1.0f, 0.0f);
        glutPostRedisplay();
    }
    else if (key == GLUT_KEY_PAGE_DOWN)
    {
        Y -= 2.0;
        glLoadIdentity();
        gluLookAt(X, Y, Z, X + X_2, Y, Z + Z_2, 0.0f, 1.0f, 0.0f);
        glutPostRedisplay();
    }

}


//keyboard key released function
void Key_Released(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        if (_moveLeftRight < 0.0f)
            _moveLeftRight = 0.0f;
        break;

    case GLUT_KEY_RIGHT:
        if (_moveLeftRight > 0.0f)
            _moveLeftRight = 0.0f;
        break;

    case GLUT_KEY_UP:
        if (_moveForeBack > 0)
            _moveForeBack = 0;

        break;

    case GLUT_KEY_DOWN:
        if (_moveForeBack < 0)
            _moveForeBack = 0;
        break;
    }
}


//timer function that rotate planets
void RotatePlanets(int value)
{
    glutPostRedisplay();
    glutTimerFunc(sec, RotatePlanets, 0);
}


//reshape function
void Reshape_Function(int width, int height)
{
    if (height == 0)
        height = 1;

    _width = width;
    _height = height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, _width, _height);
    gluPerspective(45, 1.0*(width / height), 1.0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(X, Y, Z, X + X_2, Y, Z + Z_2, 0.0f, 1.0f, 0.0f);

}


//main method
int main(int argc, char**argv)
{
    MessageBox(NULL, TEXT("Forward\t\t = \tUP\nBackward\t\t = \tDOWN\nLeft\t\t = \tLEFT\nRight\t\t = \tRIGHT\nUp\t\t = \tPAGE UP\nDown\t\t = \tPAGE DOWN\nExit\t\t = \tESC"), TEXT("How do i..........?"), MB_OK);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 500);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("3D Planetary System In OpenGL");
    glutIgnoreKeyRepeat(1);
    glutDisplayFunc(Display_Scene);
    glutKeyboardFunc(Keyboard_function);
    glutSpecialFunc(Key_Pressed);
    glutSpecialUpFunc(Key_Released);
    glutReshapeFunc(Reshape_Function);
    Init_OpenGL();
    glutTimerFunc(0, RotatePlanets, 0);
    /* you can use glutFullScreen()
    function here to go to full screen mode
    */
    glutMainLoop();
    return(0);
}
