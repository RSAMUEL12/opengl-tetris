#ifdef __APPLE__
#include <GLUT/glut.h> // if we are on a Mac
#else
#include <GL/glut.h> // on linux
#endif

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

// File to store all variables that are used by all the files in the directory

float elapsedTime = 0.0f;

#define ROWS 22
#define VISABLE_ROWS 20
#define COLUMNS 10

// tt = tetris
// sets up the rectangle for the tetris grid
const float tt_LEFT = 300;
const float tt_RIGHT = 600;
const float tt_BOTTOM = 100;
const float tt_TOP = 900;

// la = lookahead
// coordinates for drawing the lookahead box to show the next shape that will be displayed
const float la_LEFT = 100;
const float la_RIGHT = 200;
const float la_BOTTOM = 800;
const float la_TOP = 900;

// light controls
float light_ambient[] = {0.5, 0.5, 0.5, 1.0};
float light_diffuse[] = {0.2, 0.2, 0.2, 1.0};

void lights_init()
{
    // fix the light position
    float light_position[] = {1.0, 1.0, 2.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // enable lighting and turn on the light0
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // so that hidden surfaces are removed
    glEnable(GL_DEPTH_TEST);

    // mode of shading
    glShadeModel(GL_FLAT); // can be GL_FLAT, GL_SMOOTH

    // we can do this once if we don't intend to change the light properties
    // incident energy properties of a light

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}