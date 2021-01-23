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
#include "global.cpp"

const float blockHeight = (tt_TOP - tt_BOTTOM) / VISABLE_ROWS;
const float blockWidth = (tt_RIGHT - tt_LEFT) / COLUMNS;

const float laBlockHeight = (la_TOP - la_BOTTOM) / 4;
const float laBlockWidth = (la_RIGHT - la_LEFT) / 4;

// properties of some material
float mat_ambient[] = {0.1, 0.1, 0.1, 1.0};
float mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
float mat_specular[] = {0.8, 0.8, 0.8, 1.0};
float mat_shininess[] = {100.0};

// Block class is the fundamental shape for making the Tetrominos for the Tetris game
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
        // properties of some material
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
        Block::row = row;
        Block::column = column;
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
            mat_ambient[0] = 1;
            mat_ambient[1] = 1;
            mat_ambient[2] = 0;
            break;
        case (1):
            glColor3f(0.2, 1, 1);
            mat_ambient[0] = 0.2;
            mat_ambient[1] = 1;
            mat_ambient[2] = 1;
            break;
        case (2):
            glColor3f(0.17, 1, 0);
            mat_ambient[0] = 0.17;
            mat_ambient[1] = 1;
            mat_ambient[2] = 0;
            break;
        case (3):
            glColor3f(1, 0, 0);
            mat_ambient[0] = 1;
            mat_ambient[1] = 0;
            mat_ambient[2] = 0;
            break;
        case (4):
            glColor3f(1, 0.67, 0);
            mat_ambient[0] = 1;
            mat_ambient[1] = 0.67;
            mat_ambient[2] = 0;
            break;
        case (5):
            glColor3f(0, 0, 1);
            mat_ambient[0] = 0;
            mat_ambient[1] = 0;
            mat_ambient[2] = 1;
            break;
        case (6):
            glColor3f(0.7, 0.1, 1);
            mat_ambient[0] = 0.7;
            mat_ambient[1] = 0.1;
            mat_ambient[2] = 1;
            break;
        }
        // set the surface properties
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

        // if shader program is installed
        if (glIsProgram(g_program_obj))
        {
            glUseProgram(g_program_obj);

            // send the texture unit ID to fragment shader
            // it should be 0 for GL_TEXTURE0
            // it should be 1 for GL_TEXTURE1
            // it should be 2 for GL_TEXTURE2 and so on
            glUniform1i(f_tex0_loc, 0);
            glUniform1i(f_tex1_loc, 1);
        }
        else
            glEnable(GL_TEXTURE_2D); // not needed with shader

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_ceramic);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_bump_map);

        glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate
        glEnable(GL_TEXTURE_GEN_T); //enable texture coordinate

        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0f); // lower left corner
        glVertex2f(left, bottom);
        glTexCoord2f(0.0f, 1.0f); // upper left corner
        glVertex2f(left, top);
        glTexCoord2f(1.0f, 1.0f); // upper right corner
        glVertex2f(right, top);
        glTexCoord2f(1.0f, 0.0f); // lower right corner
        glVertex2f(right, bottom);
        glEnd();

        glActiveTexture(GL_TEXTURE2);

        if (glIsProgram(g_program_obj))
            glUseProgram(0);
        else
            glDisable(GL_TEXTURE_2D); // not needed with shader

        //CHECK FOR COLLISIONS AND PERFORM MOVEMENT
    }
    // overloading display() so that a block can be drawn with specific locations // for lookahead blocks
    void display(float left, float right, float bottom, float top)
    {
        switch (type)
        {
        case (0):
            glColor3f(1, 1, 0);
            mat_ambient[0] = 1;
            mat_ambient[1] = 1;
            mat_ambient[2] = 0;
            break;
        case (1):
            glColor3f(0.2, 1, 1);
            mat_ambient[0] = 0.2;
            mat_ambient[1] = 1;
            mat_ambient[2] = 1;
            break;
        case (2):
            glColor3f(0.17, 1, 0);
            mat_ambient[0] = 0.17;
            mat_ambient[1] = 1;
            mat_ambient[2] = 0;
            break;
        case (3):
            glColor3f(1, 0, 0);
            mat_ambient[0] = 1;
            mat_ambient[1] = 0;
            mat_ambient[2] = 0;
            break;
        case (4):
            glColor3f(1, 0.67, 0);
            mat_ambient[0] = 1;
            mat_ambient[1] = 0.67;
            mat_ambient[2] = 0;
            break;
        case (5):
            glColor3f(0, 0, 1);
            mat_ambient[0] = 0;
            mat_ambient[1] = 0;
            mat_ambient[2] = 1;
            break;
        case (6):
            glColor3f(0.7, 0.1, 1);
            mat_ambient[0] = 0.7;
            mat_ambient[1] = 0.1;
            mat_ambient[2] = 1;
            break;
        }

        // set the surface properties
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

        // if shader program is installed
        if (glIsProgram(g_program_obj))
        {
            glUseProgram(g_program_obj);

            // send the texture unit ID to fragment shader
            // it should be 0 for GL_TEXTURE0
            // it should be 1 for GL_TEXTURE1
            // it should be 2 for GL_TEXTURE2 and so on
            glUniform1i(f_tex0_loc, 0);
            glUniform1i(f_tex1_loc, 1);
        }
        else
            glEnable(GL_TEXTURE_2D); // not needed with shader

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_ceramic);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_bump_map);

        glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate
        glEnable(GL_TEXTURE_GEN_T); //enable texture coordinate

        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0f); // lower left corner
        glVertex2f(left, bottom);
        glTexCoord2f(0.0f, 1.0f); // upper left corner
        glVertex2f(left, top);
        glTexCoord2f(1.0f, 1.0f); // upper right corner
        glVertex2f(right, top);
        glTexCoord2f(1.0f, 0.0f); // lower right corner
        glVertex2f(right, bottom);
        glEnd();

        glActiveTexture(GL_TEXTURE2);

        if (glIsProgram(g_program_obj))
            glUseProgram(0);
        else
            glDisable(GL_TEXTURE_2D); // not needed with shader

        //lights_init();
    }
};