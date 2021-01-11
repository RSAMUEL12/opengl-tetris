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

unsigned int g_program_obj = 0;
unsigned int g_vertex_obj = 0;
unsigned int g_fragment_obj = 0;

unsigned int g_ceramic = 0;  // texture handle
unsigned int g_bump_map = 0; // texture handle
int f_tex0_loc = 0;          // location of texture in fragment shader
int f_tex1_loc = 0;          // location of texture in fragment shader