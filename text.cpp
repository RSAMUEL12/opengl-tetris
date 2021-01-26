#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include <cstring>
#include <iostream>
//using namespace std;

// This file handles the text functions and displays any text onto the display window

// get the character width to centre text in the middle of the window
int getCharacterWidth(const char *text, size_t len)
{
    int total_width = 0;
    for (size_t i = 0; i < len; i++)
    {
        total_width += glutStrokeWidth(GLUT_STROKE_ROMAN, text[i]);
    }
    return total_width;
}

// draws text supplied by the text char array argument
void draw_text(float x, float y, float size, const char *text)
{
    size = size / 100;
    size_t len = strlen(text);
    int width = getCharacterWidth(text, len);
    width = width * 0.3;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // Moves the text to the x and y values specified
    glTranslatef(x, y, 0.0f);
    glScalef(size, size, 1.0f);
    for (size_t i = 0; i < len; i++)
    {
        // draws each character in text
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
    }
    glPopMatrix();
}

// draws text that has an associated value with it e.g. score and level.
void write_key_value(float x, float y, int score, char key[])
{
    char text[10];
    sprintf(text, "%d", score);
    size_t len = strlen(text);
    int width = getCharacterWidth(text, len);
    width = width * 0.3;

    draw_text(x, y, 15.0, key);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y - 30, 0.0f); // this will work
    glScalef(0.15f, 0.15f, 1.0f);  // this will work
    for (size_t i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
    }
    glPopMatrix();
}