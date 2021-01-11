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
#include "text.cpp"
#include "tetromino.cpp"

// coordinates for the window sizes
const float LEFT = 0;
const float RIGHT = 1000;
const float BOTTOM = 0;
const float TOP = 1000;

// variables to control the game
bool isGameOver = false;
bool newBlock = true;
bool blockCreated = false;

int level = 1;
int clearedLines = 0;
int totalClearedLines = 0;
float fallSpeed = 1000;

struct materials_t
{
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
};

const materials_t g_red_plastic = {
    {0.3f, 0.0f, 0.0f, 1.0f},
    {0.6f, 0.0f, 0.0f, 1.0f},
    {0.8f, 0.6f, 0.6f, 1.0f},
    32.0f};

// properties of a given material
void set_material(const materials_t &mat)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess);
}

// class to store the Player score
class Score
{
private:
    int score;

public:
    Score()
    {
        score = 0;
    }
    int getScore()
    {
        return score;
    }
    // adds up the score
    // clearedLines are a single (1), double(2), triple(3), or tetris(4), hence the conditional
    void addScore()
    {
        int points = 0;

        if (clearedLines == 1)
        {
            points = 100 * level;
        }
        else if (clearedLines == 2)
        {
            points = 300 * level;
        }
        else if (clearedLines == 3)
        {
            points = 500 * level;
        }
        else
        {
            points = 800 * level;
        }
        score += points;
    }

    void updateLevel()
    {
        if (clearedLines != 0)
        {
            level++;
            fallSpeed = 1000 - (level * 50);
        }
        /*if (totalClearedLines == (level * 10))
        {
            level++;
            fallSpeed = fallSpeed - (level * 100);
        }*/
    }
};

Score player;

// ----------------- GLOBAL METHODS FOR GAME LOGIC --------------------------------
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

void checkGameOver()
{
    // check the rows 0 and 1 (where blocks spawn) if any blocks have landed there - first check row 1 as that will be filled first
    for (int i = 1; i >= 0; i--)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (landedBlockLocations[i][j] != 0)
            {
                isGameOver = true;
                break;
            }
        }
    }
}

// Creates the Tetris Grid where the game will be displaying the blocks
void drawLookahead(Tetromino t)
{
    // Draw the text above the lookahead piece
    draw_text(la_LEFT, la_TOP, "Next piece");

    // for each block in the shape, work out their individual left, right, top and bottom coordinates so that they can
    // be placed in the lookahead rectange space defined by la_LEFT, la_RIGHT, la_TOP and la_BOTTOM
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
            //printBoard();
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
            clearedLines++;
            totalClearedLines++;
            //printBoard();

            // add the score once the lines are cleared
            player.addScore();
            player.updateLevel();
        }
    }
    // reset the clearedLines for next time lines are cleared
    clearedLines = 0;
}

void init(int argc, char *argv[])
{
    g_ceramic = load_and_bind_texture("/Users/rohansamuel/Documents/Warwick 2020-21/CS324/cs324-labs/images/tile.png");
    //g_bump_map = load_and_bind_texture("../images/normal.png");
    g_bump_map = load_and_bind_texture("/Users/rohansamuel/Documents/Warwick 2020-21/CS324/cs324-labs/images/chesterfield.png");

    int max_texture_units = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
    fprintf(stderr, "Max texture units is %d\n", max_texture_units);

    if (argc > 2)
    {
        g_program_obj = create_and_compile_shaders(argv[1], NULL, argv[2]);

        // get the location of the fragment shader texture variable
        f_tex0_loc = glGetUniformLocation(g_program_obj, "f_tex0");
        f_tex1_loc = glGetUniformLocation(g_program_obj, "f_tex1");
    }

    //init_lights();

    //set_material(g_red_plastic);

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Defines 2d orthographic projection viewing
    // left, right, bottom, top
    gluOrtho2D(LEFT, RIGHT, BOTTOM, TOP);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Tetromino currentTetro;
Tetromino nextTetro(true);
int totalTetros = 0;

// TODO: Consider adding a lock delay to last minute rotations
void playGame()
{
    if (!isGameOver)
    {
        if (newBlock)
        {
            //printBoard();
            //printf("New Block made...\n");
            Tetromino tetro(true);
            currentTetro = nextTetro;
            totalTetros++;
            newBlock = false;
            nextTetro = tetro;
        }

        draw_score(la_LEFT, tt_BOTTOM, player.getScore());
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
        checkGameOver();
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
    else
    {
        draw_text(350, 350, "GAME OVER!");
    }
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

void timer_func(int n)
{
    glutPostRedisplay();
    elapsedTime++;
    glutTimerFunc(fallSpeed, timer_func, 0);
}

void special(int key, int, int)
{
    // keyboard controls are enabled once there is a block being created to avoid Segmentation faults
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
    init(argc, argv);
    srand(time(0));
    glutDisplayFunc(display);
    //glutIdleFunc(idle);
    glutTimerFunc(fallSpeed, timer_func, 0);

    // handlers for keyboard input
    //glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    glutMainLoop();

    return 0;
}