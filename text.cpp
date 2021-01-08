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

#include <stddef.h>
#include <string>
#include <iostream>
using namespace std;

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

void draw_text(float x, float y, const char *text)
{
    size_t len = strlen(text);
    int width = getCharacterWidth(text, len);
    width = width * 0.3;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);     // this will work
    glScalef(0.15f, 0.15f, 1.0f); // this will work
    for (size_t i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
    }
    glPopMatrix();
}

void draw_score(float x, float y, int score)
{
    char text[10];
    std::sprintf(text, "%d", score);
    size_t len = strlen(text);
    int width = getCharacterWidth(text, len);
    width = width * 0.3;

    draw_text(x, y, "Score:");

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