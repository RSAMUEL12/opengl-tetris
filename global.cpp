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
const float startingSpeed = 1000;
const float noDelaySpeed = 0;
float fallSpeed = 1000;
float currentSpeed = 0;

#define ROWS 22
#define VISABLE_ROWS 20
#define COLUMNS 10

// tt = tetris
// sets up the rectangle for the tetris grid
const float tt_LEFT = 500;
const float tt_RIGHT = 900;
const float tt_BOTTOM = 100;
const float tt_TOP = 900;

// la = lookahead
// coordinates for drawing the lookahead box to show the next shape that will be displayed
const float la_LEFT = 100;
const float la_RIGHT = 250;
const float la_BOTTOM = 750;
const float la_TOP = 900;

unsigned int g_program_obj = 0;
unsigned int g_vertex_obj = 0;
unsigned int g_fragment_obj = 0;

int g_ticks = 0; // keeps couting

unsigned int g_ceramic = 0;  // texture handle
unsigned int g_bump_map = 0; // texture handle
int f_tex0_loc = 0;          // location of texture in fragment shader
int f_tex1_loc = 0;          // location of texture in fragment shader

// light controls
float light_ambient[] = {0.5, 0.5, 0.5, 1.0};
float light_diffuse[] = {0.2, 0.2, 0.2, 1.0};
float light_specular[] = {0.8, 0.8, 0.8, 1.0};

void lights_init()
{
    // fix the light position
    float light_position[] = {1.0, 0.6, 2.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // we can do this once if we don't intend to change the light properties
    // incident energy properties of a light

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);

    glFrontFace(GL_CW);

    // enable lighting and turn on the light0
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    // so that hidden surfaces are removed
    glEnable(GL_DEPTH_TEST);

    // mode of shading
    glShadeModel(GL_FLAT); // can be GL_FLAT, GL_SMOOTH

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void material_init()
{
    float mat_ambient[] = {1.0, 1.0, 1.0, 1.0};
    float mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
    float mat_specular[] = {0.8, 0.8, 0.8, 1.0};
    float mat_shininess[] = {100.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}