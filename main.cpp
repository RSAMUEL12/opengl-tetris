#ifdef __APPLE__
#include <GLUT/glut.h> // if we are on a Mac
#else
#include <GL/glut.h> // on linux
#endif

#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 22
#define VISABLE_ROWS 20
#define COLUMNS 10

const char shapes[] = {'I', 'J', 'L', 'O', 'S', 'Z', 'T'};
int boardLocations[ROWS][COLUMNS] = {0};

const float LEFT = 0;
const float RIGHT = 1000;
const float BOTTOM = 0;
const float TOP = 1000;

// tt = tetris
// sets up the rectangle for the tetris grid
const float tt_LEFT = 300;
const float tt_RIGHT = 600;
const float tt_BOTTOM = 100;
const float tt_TOP = 900;

// Tetronmino shapes that will be chosen at random
const bool tetroShapes[28][4] =
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 0, 1, 1},
        {0, 1, 1, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 1, 0},
        {0, 0, 0, 0},
};

const float blockHeight = (tt_TOP - tt_BOTTOM) / VISABLE_ROWS;
const float blockWidth = (tt_RIGHT - tt_LEFT) / COLUMNS;

bool newBlock = false;

// Block class is the fundamental shape for making the Tetronimos for the Tetris game
class Block
{
private:
    int row;
    int column;
    float left;
    float right;
    float bottom;
    float top;

public:
    // Takes the row and column from the matrix that represents the tetris grid
    // and creates the left, right, top and bottom coordinates
    Block() {}

    Block(int row, int column)
    {
        Block::row = row;
        Block::column = column;
        left = tt_LEFT + (blockWidth * (column - 1));
        right = tt_LEFT + (blockWidth * (column));
        bottom = tt_BOTTOM + (blockHeight * (VISABLE_ROWS - row));
        top = tt_BOTTOM + (blockHeight * (VISABLE_ROWS - (row + 1)));
    }
    void getCoords()
    {
        printf("width: %f \n", blockWidth);
        printf("LEFT: %f \n", left);
        printf("RIGHT: %f \n", right);
        printf("BOTTOM: %f \n", bottom);
        printf("TOP: %f \n", top);
    }
    void display()
    {
        glColor3f(1, 1, 0);
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(left, top);
        glVertex2f(right, top);
        glVertex2f(right, bottom);
        glEnd();

        //CHECK FOR COLLISIONS AND PERFORM MOVEMENT
    }
};

// Tetronimo class are the different shapes that can appear and be placed on the grid
// They are made up of Blocks
class Tetronimo
// Tetronimo is generated by choosing a random shape from the 7 selected
// check where the shape will fit in boardLocations and pass the row and column number to each new Block object - store in blocks
{
private:
    int shape[4][4];
    Block blocks; //stores pointers to its corresponding blocks, each block has their own coordinates
    int type;

public:
    Tetronimo() {}
    void create()
    {
        srand(time(0));
        type = rand() % 7;
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                shape[y][x] = tetroShapes[(type * 4) + y][x];
            }
        }
    }
    void showShape()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << " " << shape[i][j] << " ";
            }
            std::cout << "\n";
        }
    }
};

Block blockLocations[ROWS][COLUMNS] = {};

char randomSelect()
{
    int randIndex = rand() % 7;
    return shapes[randIndex];
}

// Creates the Tetris Grid where the game will be displaying the blocks
void drawGrid()
{
    // creates vertical lines
    for (int i = 0; i <= 10; i++)
    {
        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2f(tt_LEFT + ((tt_RIGHT - tt_LEFT) / 10) * i, tt_BOTTOM);
        glVertex2f(tt_LEFT + ((tt_RIGHT - tt_LEFT) / 10) * i, tt_TOP);
        glEnd();
    }

    // creates horizontal lines
    for (int i = 0; i <= 20; i++)
    {
        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2f(tt_LEFT, tt_BOTTOM + ((tt_TOP - tt_BOTTOM) / 20) * i);
        glVertex2f(tt_RIGHT, tt_BOTTOM + ((tt_TOP - tt_BOTTOM) / 20) * i);
        glEnd();
    }
}

void drawBlock()
{
    int row = 5;
    int column = 10;
    if (boardLocations[row][column] == 0)
    {
        Block block(row, column);
        blockLocations[row][column] = block;
        boardLocations[row][column] = 1;
    }
    blockLocations[row][column].display();
}

void createTetromino()
{
    Tetronimo tetro;
    tetro.create();
    tetro.showShape();
}

void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Defines 2d orthographic projection viewing
    // left, right, bottom, top
    gluOrtho2D(LEFT, RIGHT, BOTTOM, TOP);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

// redraw callback
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    //drawGrid();
    glColor3f(1, 1, 1);
    drawGrid();
    drawBlock();
    createTetromino();
    glutSwapBuffers(); // swap the backbuffer with the front
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Tetris");
    init();
    glutDisplayFunc(display);

    // handlers for keyboard input
    //glutKeyboardFunc(keyboard);
    //glutSpecialFunc(special);

    glutMainLoop();

    return 0;
}