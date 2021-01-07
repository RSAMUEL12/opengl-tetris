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

// la = lookahead
// coordinates for drawing the lookahead box to show the next shape that will be displayed
const float la_LEFT = 100;
const float la_RIGHT = 200;
const float la_BOTTOM = 800;
const float la_TOP = 900;

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

const float laBlockHeight = (la_TOP - la_BOTTOM) / 4;
const float laBlockWidth = (la_RIGHT - la_LEFT) / 4;

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
    // overloading display() so that a block can be drawn with specific locations // for lookahead blocks
    void display(float left, float right, float bottom, float top)
    {
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
    }
};
Block blockLocations[ROWS][COLUMNS];

// Tetronimo class are the different shapes that can appear and be placed on the grid
// They are made up of Blocks
class Tetronimo
// Tetronimo is generated by choosing a random shape from the 7 selected
// check where the shape will fit in boardLocations and pass the row and column number to each new Block object - store in blocks
{
private:
    int original[4][4]; //used for rotation to store the original shape
    int type;
    int size;
    int columnDiff;
    int rowDiff;

public:
    int shape[4][4];
    Block blocks[4]; //stores tetromino's corresponding blocks, each block has their own coordinates
    // used for array initialisation of type Tetronimo
    Tetronimo(){};
    // create when you want to construct a tetro
    Tetronimo(bool x)
    {
        create();
        spawnShape(-2, 3);
    };
    void create()
    {
        type = (rand() % 7);
        size = 4;
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                shape[y][x] = tetroShapes[(type * 4) + y][x];
                original[y][x] = tetroShapes[(type * 4) + y][x];
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
        columnDiff = 0;
        rowDiff = 0;
        while (index < size)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (shape[i][j] == 1)
                    {
                        // translate the shape onto the grid using "row" and "column" values
                        // they are either default of -2 and 3 for spawn points
                        // or, they are specified if a shape is rotated and needs to be respawned in a new location
                        Coordinate c;
                        c = performTranslation(row, column, i, j);
                        //printf("%d, %d |", c.row, c.column);
                        Block block(c.row, c.column, type);
                        blocks[index] = block;
                        index++;
                    }
                }
            }
        }
        printf("\n");
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

    Coordinate performTranslation(int row, int column, int i, int j)
    {
        // if there is a block on the row and column specified, then do not rotate
        int newRow = 0;
        int newColumn = 0;

        int blockRow = row + i;

        int blockColumn = column + j;
        //printf(" %d, %d |", blockRow, blockColumn);
        // columnDiff and rowDiff ensure that if the rotate block appears outside of the board,
        // it will move it onto the board by finding the distance from the outside block to the boundary
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

        newRow = blockRow + rowDiff;
        newColumn = blockColumn + columnDiff;
        Coordinate coord;
        //printf(" %d, %d |", newRow, newColumn);
        coord.row = newRow;
        coord.column = newColumn;
        return coord;
    }
    // function to check if the rotated shape will collide or overlap with any landed blocks
    bool checkRotationCollision(int row, int column)
    {
        // loop through the shape matrix
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (shape[i][j] != 0)
                {
                    Coordinate c;
                    c = performTranslation(row, column, i, j);
                    // retrieve the translated coords of the block after rotation
                    // check if it overlaps with any existing landed blocks
                    // if so, return true so that the shape can be reverted to the original
                    if (c.row >= 0)
                    {
                        if (landedBlockLocations[c.row][c.column] != 0)
                        {
                            return true;
                        }
                    }
                    else if (c.column > 9 or c.column < 0)
                    {
                        return true;
                    }
                    else if (c.row > 21)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
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
            blockLocations[row][column] = blocks[i];
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
        /*std::cout << "\nMatrix after rotating 90 degree clockwise:\n";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << shape[i][j] << " ";
            }
            std::cout << "\n";
        }*/
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

        // check if a collision occurs with the new shape by translating it onto the
        // tetris board, and if so, revert the shape back to the original
        if (checkRotationCollision(diffRows, diffColumns))
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    shape[i][j] = original[i][j];
                }
            }
        }
        // continue with the rotation
        else
        {
            // use the translation matrix on the indices of the shape variable to move the blocks in the correct
            // position for rotation
            spawnShape(diffRows, diffColumns);
        }
    }

    // Display function calls the Block display function to show the tetromino
    void display()
    {
        for (int i = 0; i < size; i++)
        {
            blocks[i].display();
        }
    }

    // Display for overloaded display() func of block - used for lookahead blocks
    void display(float left, float right, float bottom, float top)
    {
        for (int i = 0; i < size; i++)
        {
            blocks[i].display(left, right, bottom, top);
        }
    }
};

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

// Creates the Tetris Grid where the game will be displaying the blocks
void drawLookahead(Tetronimo t)
{
    int index = 0;
    float left;
    float right;
    float top;
    float bottom;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (t.shape[i][j] != 0)
            {
                left = la_LEFT + (laBlockWidth * (j));
                right = la_LEFT + (laBlockWidth * (j + 1));
                bottom = la_BOTTOM + (laBlockHeight * (4 - i));
                top = la_BOTTOM + (laBlockHeight * (4 - (i + 1)));
                t.blocks[index].display(left, right, bottom, top);
                index++;
            }
        }
    }
}

// Clear any rows that are completely filled
void clearLines()
{
    // goes through all the rows to see if any lines need to be cleared
    for (int i = 0; i < ROWS; i++)
    {
        if (landedBlockLocations[i][0] != 0 and landedBlockLocations[i][1] != 0 and landedBlockLocations[i][2] != 0 and landedBlockLocations[i][3] != 0 and landedBlockLocations[i][4] != 0 and landedBlockLocations[i][5] != 0 and landedBlockLocations[i][6] != 0 and landedBlockLocations[i][7] != 0 and landedBlockLocations[i][8] != 0 and landedBlockLocations[i][9] != 0)
        {
            printBoard();
            printf("i: %d\n", i);
            // clear the line if they are all full
            for (int g = i - 1; g > 0; g--)
            {
                //printf("%d\n", g + 1);
                for (int h = 0; h < COLUMNS; h++)
                {
                    landedBlockLocations[g + 1][h] = landedBlockLocations[g][h];
                    blockLocations[g + 1][h] = blockLocations[g][h];
                    blockLocations[g + 1][h].setRowColumn(g + 1, h);
                }
                printf("row of last value: %d\n", blockLocations[i][0].getRow());
            }

            for (int h = 0; h < COLUMNS; h++)
            {
                landedBlockLocations[0][h] = 0;
                Block block;
                blockLocations[0][h] = block;
            }

            printBoard();
        }
    }
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

Tetronimo currentTetro;
Tetronimo nextTetro(true);
int totalTetros = 0;

void playGame()
{
    while (newBlock)
    {
        //printBoard();
        printf("New Block made...\n");
        Tetronimo tetro(true);
        currentTetro = nextTetro;
        totalTetros++;
        newBlock = false;
        nextTetro = tetro;
    }

    drawLookahead(nextTetro);
    // display all the blocks on the grid
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (landedBlockLocations[i][j] != 0)
            {
                blockLocations[i][j].display();
            }
        }
    }

    currentTetro.fall();
    if (currentTetro.isBottom() or currentTetro.isCollision())
    {
        currentTetro.setLanded();
        newBlock = true;
    };
    clearLines();
    currentTetro.display();
    blockCreated = true;

    /*for (int i = 0; i < totalTetros; i++)
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
    }*/
}

void tester()
{
    Tetronimo t(true);
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
            currentTetro.moveLeft();
            currentTetro.display();
            break;
        case GLUT_KEY_RIGHT:
            currentTetro.moveRight();
            currentTetro.display();
            break;
        case GLUT_KEY_UP:
            // Rotate shape
            currentTetro.rotateRight();
            currentTetro.display();
            break;
        case GLUT_KEY_DOWN:
            currentTetro.moveDownwards();
            currentTetro.display();
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