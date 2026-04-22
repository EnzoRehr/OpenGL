#include <GL/freeglut.h>
#include <cmath>
#include "lighting.h"

#ifndef M_PI
#define M_PI 3.14159265f
#endif

static float sunX = 50.0f;
static float sunY = 100.0f;
static float sunZ = 50.0f;

// Pozitiile stalpilor (la colturi circuit)
static float polePos[4][3] = {
    { -28.0f, 0.0f, -18.0f },
    {  28.0f, 0.0f, -18.0f },
    { -28.0f, 0.0f,  18.0f },
    {  28.0f, 0.0f,  18.0f }
};
static float poleHeight = 8.0f;

void lightingInit() {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    // LIGHT0: Soare
    glEnable(GL_LIGHT0);
    GLfloat ambSun[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffSun[] = { 1.0f, 0.95f, 0.8f, 1.0f };
    GLfloat specSun[] = { 0.5f, 0.5f,  0.5f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambSun);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffSun);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specSun);

    // LIGHT1-4: Stalpi stradali
    GLenum lights[] = { GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    GLfloat ambPole[] = { 0.1f, 0.08f, 0.02f, 1.0f };
    GLfloat diffPole[] = { 1.0f, 0.85f, 0.4f,  1.0f };
    GLfloat specPole[] = { 0.3f, 0.3f,  0.1f,  1.0f };
    for (int i = 0; i < 4; i++) {
        glEnable(lights[i]);
        glLightfv(lights[i], GL_AMBIENT, ambPole);
        glLightfv(lights[i], GL_DIFFUSE, diffPole);
        glLightfv(lights[i], GL_SPECULAR, specPole);
        glLightf(lights[i], GL_CONSTANT_ATTENUATION, 0.3f);
        glLightf(lights[i], GL_LINEAR_ATTENUATION, 0.05f);
        glLightf(lights[i], GL_QUADRATIC_ATTENUATION, 0.002f);
    }
}

void lightingApply() {
    GLfloat sunPos[] = { sunX, sunY, sunZ, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, sunPos);

    GLenum lights[] = { GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4 };
    for (int i = 0; i < 4; i++) {
        // Lumina la varful stalpului
        GLfloat pos[] = { polePos[i][0], poleHeight, polePos[i][2], 1.0f };
        glLightfv(lights[i], GL_POSITION, pos);
    }
}

// Deseneaza stalpii vizual cu glutSolidCube (fara cilindri)
void lightingDrawPoles() {
    glDisable(GL_TEXTURE_2D);

    for (int i = 0; i < 4; i++) {
        float x = polePos[i][0];
        float z = polePos[i][2];

        // Stalp vertical (cub alungit)
        glColor3f(0.2f, 0.2f, 0.2f);
        glPushMatrix();
        glTranslatef(x, poleHeight / 2.0f, z);
        glScalef(0.3f, poleHeight, 0.3f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Brat orizontal
        glPushMatrix();
        glTranslatef(x + 0.75f, poleHeight, z);
        glScalef(1.5f, 0.2f, 0.2f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Bec (sfera galbena mica)
        glColor3f(1.0f, 0.95f, 0.5f);
        glPushMatrix();
        glTranslatef(x + 1.5f, poleHeight, z);
        glutSolidSphere(0.3f, 8, 8);
        glPopMatrix();
    }

    glEnable(GL_TEXTURE_2D);
}

void lightingGetSunPos(float& x, float& y, float& z) {
    x = sunX; y = sunY; z = sunZ;
}

void lightingMakeShadowMatrix(float plane[4], float light[4], float mat[16]) {
    float dot = plane[0] * light[0] + plane[1] * light[1]
        + plane[2] * light[2] + plane[3] * light[3];

    mat[0] = dot - light[0] * plane[0]; mat[4] = -light[0] * plane[1];
    mat[8] = -light[0] * plane[2]; mat[12] = -light[0] * plane[3];
    mat[1] = -light[1] * plane[0]; mat[5] = dot - light[1] * plane[1];
    mat[9] = -light[1] * plane[2]; mat[13] = -light[1] * plane[3];
    mat[2] = -light[2] * plane[0]; mat[6] = -light[2] * plane[1];
    mat[10] = dot - light[2] * plane[2]; mat[14] = -light[2] * plane[3];
    mat[3] = -light[3] * plane[0]; mat[7] = -light[3] * plane[1];
    mat[11] = -light[3] * plane[2]; mat[15] = dot - light[3] * plane[3];
}