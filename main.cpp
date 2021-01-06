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

#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define ROWS 22
#define VISABLE_ROWS 20
#define COLUMNS 10

const char shapes[] = {'I', 'J', 'L', 'O', 'S', 'Z', 'T'};
int boardLocations[ROWS][COLUMNS] = {0};
int landedBlockLocations[ROWS][COLUMNS] = {0};
float elapsedTime = 0.0f;

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

struct Coordinate
{
    int row;
    int column;
};

struct Colour
{
    float r;
    float g;
    float b;
};

Colour colours[7] = {};
// Tetronmino shapes that will be chosen at random
const bool tetroShapes[28][4] =
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {2, 2, 2, 2},
        {0, 0, 0, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 0, 3, 3},
        {0, 3, 3, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 4, 4, 0},
        {0, 0, 4, 4},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 5, 5, 5},
        {0, 5, 0, 0},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 6, 6, 6},
        {0, 0, 0, 6},
        {0, 0, 0, 0},

        {0, 0, 0, 0},
        {0, 7, 7, 7},
        {0, 0, 7, 0},
        {0, 0, 0, 0},
};

const float blockHeight = (tt_TOP - tt_BOTTOM) / VISABLE_ROWS;
const float blockWidth = (tt_RIGHT - tt_LEFT) / COLUMNS;

bool newBlock = true;
bool blockCreated = false;

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
    int type;

public:
    // Takes the row and column from the matrix that represents the tetris grid
    // and creates the left, right, top and bottom coordinates
    Block() {}

    Block(int row, int column, int type)
    {
        Block::row = row;
        Block::column = column;
        Block::type = type;
        resetCoords();
    }
    void getCoords()
    {
        printf("width: %f \n", blockWidth);
        printf("LEFT: %f \n", left);
        printf("RIGHT: %f \n", right);
        printf("BOTTOM: %f \n", bottom);
        printf("TOP: %f \n", top);
    }
    int getRow()
    {
        return Block::row;
    }
    int getColumn()
    {
        return Block::column;
    }
    void setRowColumn(int row, int column)
    {
        boardLocations[Block::row][Block::column] = 0;
        Block::row = row;
        Block::column = column;
        boardLocations[Block::row][Block::column] = 1;
        resetCoords();
    }
    void resetCoords()
    {
        left = tt_LEFT + (blockWidth * (column));
        right = tt_LEFT + (blockWidth * (column + 1));
        bottom = tt_BOTTOM + (blockHeight * (ROWS - row));
        top = tt_BOTTOM + (blockHeight * (ROWS - (row + 1)));
    }
    void display()
    {
        if (row < 2)
        {
            return;
        }
        //printf("%d, %d", row, column);
        // Selecting the colour based on the shape type
        switch (type)
        {
        case (0):
            glColor3f(1, 1, 0);
            break;
        case (1):
            glColor3f(0.2, 1, 1);
            break;
        case (2):
            glColor3f(0.17, 1, 0);
            break;
        case (3):
            glColor3f(1, 0, 0);
            break;
        case (4):
            glColor3f(1, 0.67, 0);
            break;
        case (5):
            glColor3f(0, 0, 1);
            break;
        case (6):
            glColor3f(0.7, 0.1, 1);
            break;
        }
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(left, top);
        glVertex2f(right, top);
        glVertex2f(right, bottom);
        glEnd();

        //CHECK FOR COLLISIONS AND PERFORM MOVEMENT
    }
};

void rotateMatrix(int arr[4][4])
{
    int n = 4;
    for (int i = 0; i < n / 2; i++)
    {
        for (int j = i; j < n - i - 1; j++)
        {
            // Swapping elements after each iteration in Clockwise direction
            int temp = arr[i][j];
            arr[i][j] = arr[n - 1 - j][i];
            arr[n - 1 - j][i] = arr[n - 1 - i][n - 1 - j];
            arr[n - 1 - i][n - 1 - j] = arr[j][n - 1 - i];
            arr[j][n - 1 - i] = temp;
        }
    }
}

// Tetronimo class are the different shapes that can appear and be placed on the grid
// They are made up of Blocks
class Tetronimo
// Tetronimo is generated by choosing a random shape from the 7 selected
// check where the shape will fit in boardLocations and pass the row and column number to each new Block object - store in blocks
{
private:
    int shape[4][4];
    Block blocks[4]; //stores tetromino's corresponding blocks, each block has their own coordinates
    int type;
    int size;

public:
    Tetronimo()
    {
        create();
        spawnShape(-2, 3);
    }
    void create()
    {
        type = (rand() % 7);
        size = 4;
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
    void spawnShape(int row, int column)
    {
        int index = 0;
        int columnDiff = 0;
        int rowDiff = 0;
        while (index < size)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (shape[i][j] == 1)
                    {
                        int blockRow = row + i;
                        int blockColumn = column + j;
                        // if called after a rotation, ensure that the new row, column values are within the board by moving the piece
                        // by how far the furthest piece is away from the board, and add this difference to the row
                        if (blockColumn < 0 and columnDiff == 0)
                        {
                            columnDiff = 0 - blockColumn;
                        }
                        else if (blockColumn > 10 and columnDiff == 0)
                        {
                            columnDiff = 10 - blockColumn; //should be a neg value added to all the columns
                        }

                        if (blockRow < 0 and rowDiff == 0)
                        {
                            rowDiff = 0 - blockRow;
                        }
                        else if (row > 22 and rowDiff == 0)
                        {
                            rowDiff = 10 - blockRow; //should be a neg value added to all the columns
                        }
                        printf("%d", blockRow + rowDiff);
                        Block block(blockRow + rowDiff, blockColumn + columnDiff, type);
                        blocks[index] = block;
                        index++;
                    }
                }
            }
        }
    }

    bool checkLeft()
    {
        for (int i = 0; i < size; i++)
        {
            int row = blocks[i].getRow();
            int column = blocks[i].getColumn();
            if (column <= 0)
            {
                return false;
            }
            else
            {
                if (landedBlockLocations[row][column - 1] != 0)
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool checkRight()
    {
        for (int i = 0; i < size; i++)
        {
            int row = blocks[i].getRow();
            int column = blocks[i].getColumn();
            if (column >= 9)
            {
                return false;
            }
            else
            {
                if (landedBlockLocations[row][column + 1] != 0)
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool checkDown()
    {
        for (int i = 0; i < size; i++)
        {
            int row = blocks[i].getRow();
            if (row >= 21)
            {
                return false;
            }
        }
        return true;
    }

    bool isBottom()
    {
        for (int i = 0; i < size; i++)
        {
            int column = blocks[i].getColumn();
            int row = blocks[i].getRow();
            if (row == 21)
            {
                setLanded();
                return true;
            }
            else if ((row != 21 and column != 9))
            {
                if (landedBlockLocations[row + 1][column] != 0)
                {
                    setLanded();
                    return true;
                }
            }
        }
        return false;
    }

    void setLanded()
    {
        //printf("Landed...\n");
        for (int i = 0; i < size; i++)
        {
            int row = blocks[i].getRow();
            int column = blocks[i].getColumn();
            landedBlockLocations[row][column] = 1;
        }
    }

    // Check if there is a collision with another block from another tetronimo
    bool isCollision()
    {
        for (int i = 0; i < size; i++)
        {
            int row = blocks[i].getRow();
            int column = blocks[i].getColumn();
            // Check if there is a collision with another block below
            if (row != 21)
            {
                if (landedBlockLocations[row + 1][column] != 0)
                {
                    printf("Collision detected...\n");
                    return true;
                }
            }
        }
        return false;
    }
    //printf("\n");

    void
    moveLeft()
    {
        if (checkLeft() == true and isCollision() == false)
        {
            for (int i = 0; i < size; i++)
            {
                // Check if a boundary is hit
                blocks[i].setRowColumn(blocks[i].getRow(), blocks[i].getColumn() - 1);
                blocks[i].display();
            }
        }
    }

    void moveRight()
    {
        if (checkRight() == true and isCollision() == false)
        {
            for (int i = 0; i < size; i++)
            {
                // Check if a boundary is hit
                blocks[i].setRowColumn(blocks[i].getRow(), blocks[i].getColumn() + 1);
                blocks[i].display();
            }
        }
    }

    void fall()
    {
        if (checkDown() == true and elapsedTime > 0 and isCollision() == false)
        {
            for (int i = 0; i < size; i++)
            {
                // Check if a boundary is hit
                int row = blocks[i].getRow();
                int column = blocks[i].getColumn();
                blocks[i].setRowColumn(row + 1, column);
                //printf("%d, %d\n", row, column);
                blocks[i].display();
            }
        }
        if (elapsedTime > 0)
        {
            elapsedTime--;
        }
    }

    void moveDownwards()
    {
        if (checkDown() == true and isCollision() == false)
        {
            for (int i = 0; i < size; i++)
            {
                // Check if a boundary is hit
                int row = blocks[i].getRow();
                int column = blocks[i].getColumn();
                blocks[i].setRowColumn(row + 1, column);
                //printf("%d, %d\n", row, column);
                blocks[i].display();
            }
        }
        if (elapsedTime > 0)
        {
            elapsedTime--;
        }
    }

    void rotateRight()
    {
        int n = 4;
        int original[4][4];
        // copy the original shape to an array so it can be used later
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                original[i][j] = shape[i][j];
            }
        }
        for (int i = 0; i < n / 2; i++)
        {
            for (int j = i; j < n - i - 1; j++)
            {
                // Swapping elements after each iteration in Clockwise direction
                int temp = shape[i][j];
                shape[i][j] = shape[n - 1 - j][i];
                shape[n - 1 - j][i] = shape[n - 1 - i][n - 1 - j];
                shape[n - 1 - i][n - 1 - j] = shape[j][n - 1 - i];
                shape[j][n - 1 - i] = temp;
            }
        }
        // Printing matrix elements after rotation
        std::cout << "\nMatrix after rotating 90 degree clockwise:\n";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << shape[i][j] << " ";
            }
            std::cout << "\n";
        }
        int diffRows = 0;
        int diffColumns = 0;
        int index = 0;
        // Calculate the translation matrix from the original shape matrix and the current block rows and columns
        // to translate the new shape onto the blockLocations matrix with the correct row and column numbers.
        while (index < size)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (original[i][j] != 0)
                    {
                        diffRows = blocks[index].getRow() - i;
                        diffColumns = blocks[index].getColumn() - i;
                        index++;
                    }
                }
            }
        }

        // use the translation matrix on the indices of the shape variable to move the blocks in the correct
        // position for rotation
        spawnShape(diffRows, diffColumns);
    }

    // Display function calls the Block display function to show the tetromino
    void display()
    {
        for (int i = 0; i < size; i++)
        {
            blocks[i].display();
        }
    }
};

Block blockLocations[ROWS][COLUMNS] = {};

char randomSelect()
{
    int randIndex = rand() % 7;
    return shapes[randIndex];
}

void printBoard()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            printf("%d ", landedBlockLocations[i][j]);
        }
        printf("\n");
    }
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

// Clear any rows that are completely filled
void clearLines()
{
    //TODO: Can redo how blocks are stored and create pointers to blocks when forming a Tetromino, whilst having an array
    // storing the locations of blocks with each element a Block object.
    // 1. So when it comes to clearing lines, check for an element
    // in the row that equal to 0, then you cannot clear the line and move on
    // 2. If a line can be cleared, change the element of an array from 1 to 0 in boardLocations, delete the Block element in blockLocations
    // which should delete the values in the Tetromino blocks array -> reduce size by 1
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

Tetronimo allTetros[10];
int totalTetros = 0;

void playGame()
{
    while (newBlock)
    {
        //printBoard();
        printf("New Block made...\n");
        Tetronimo tetro;
        allTetros[totalTetros] = tetro;
        totalTetros++;
        newBlock = false;
    }

    for (int i = 0; i < totalTetros; i++)
    {
        if (i == totalTetros - 1)
        {
            allTetros[i].fall();
            if (allTetros[i].isBottom() or allTetros[i].isCollision())
            {
                allTetros[i].setLanded();
                newBlock = true;
            };
        }
        allTetros[i].display();
        blockCreated = true;
    }
}

void tester()
{
    Tetronimo t;
    t.rotateRight();
    printf("\n");
    t.rotateRight();
    printf("\n");
    t.rotateRight();
    printf("\n");
    t.rotateRight();
    printf("\n");
    exit(0);
}

// redraw callback
void display()
{
    //printf("Calling Display");
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);
    drawGrid();
    playGame();
    //tester();
    glutSwapBuffers(); // swap the backbuffer with the front
    //printf("Display Done...\n");
}

void idle()
{
    //printf("Calling Idle \n");
    usleep(1000000);
    elapsedTime++;
    glutPostRedisplay();
}

void timer_func(int n)
{
    glutPostRedisplay();
    elapsedTime++;
    glutTimerFunc(1000, timer_func, 0);
}

void special(int key, int, int)
{
    if (blockCreated)
    {
        // handle special keys
        switch (key)
        {
        case GLUT_KEY_LEFT:
            allTetros[totalTetros - 1].moveLeft();
            allTetros[totalTetros - 1].display();
            break;
        case GLUT_KEY_RIGHT:
            allTetros[totalTetros - 1].moveRight();
            allTetros[totalTetros - 1].display();
            break;
        case GLUT_KEY_UP:
            // Rotate shape
            allTetros[totalTetros - 1].rotateRight();
            allTetros[totalTetros - 1].display();
            break;
        case GLUT_KEY_DOWN:
            allTetros[totalTetros - 1].moveDownwards();
            allTetros[totalTetros - 1].display();
            break;
        }
    }

    //elapsedTime++;
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Tetris");
    init();
    srand(time(0));
    glutDisplayFunc(display);
    //glutIdleFunc(idle);
    glutTimerFunc(1000, timer_func, 0);

    // handlers for keyboard input
    //glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glutMainLoop();

    return 0;
}