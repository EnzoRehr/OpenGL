#include <GL/freeglut.h>
#include <cmath>
#include "car.h"
#include "lighting.h"

#ifndef M_PI
#define M_PI 3.14159265f
#endif

static float carX = 0.0f;
static float carZ = -17.5f;
static float carAngle = 0.0f;  // radiani, 0 = priveste spre +X
static float carSpeed = 0.3f;

struct AABB { float minX, maxX, minZ, maxZ; };
static AABB buildings[] = {
    {-43,-37, -6,  6}, {-43,-37, 12, 21}, {-43,-37,-21,-12},
    { 37, 43, -6,  6}, { 37, 43, 12, 21}, { 37, 43,-21,-12},
};
static int numBuildings = 6;

static bool checkCollision(float x, float z, float r) {
    for (int i = 0; i < numBuildings; i++)
        if (x + r > buildings[i].minX && x - r < buildings[i].maxX &&
            z + r > buildings[i].minZ && z - r < buildings[i].maxZ)
            return true;
    return false;
}

void carInit() { carX = 0.0f; carZ = -17.5f; carAngle = 0.0f; }

void carSpecialKeys(int key) {
    float nx = carX, nz = carZ;
    // carAngle 0 = spre +Z (inainte)
    if (key == GLUT_KEY_UP) { nx += sinf(carAngle) * carSpeed; nz += cosf(carAngle) * carSpeed; }
    if (key == GLUT_KEY_DOWN) { nx -= sinf(carAngle) * carSpeed; nz -= cosf(carAngle) * carSpeed; }
    if (key == GLUT_KEY_LEFT)  carAngle -= 0.05f;
    if (key == GLUT_KEY_RIGHT) carAngle += 0.05f;
    if (!checkCollision(nx, nz, 1.2f)) { carX = nx; carZ = nz; }
    glutPostRedisplay();
}

// Geometrie masina: fata = +Z, dreapta = +X, sus = +Y
// Corp: 2 unitati lat (X), 0.7 inalt (Y), 2.2 lung (Z)
void drawCarBody(float r, float g, float b) {
    // Corp principal - culcat pe Z
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    glScalef(1.8f, 0.7f, 2.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Cabina - mai mica, centrata, decalata spre spate
    glColor3f(r * 0.85f, g * 0.85f, b * 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, -0.1f);
    glScalef(1.5f, 0.6f, 1.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Roti: pe laturile X (stanga/dreapta), fata/spate pe Z
    glColor3f(0.1f, 0.1f, 0.1f);
    // pozitii: (±X, Y, ±Z)
    float wx[] = { -1.0f,  1.0f, -1.0f,  1.0f };
    float wz[] = { -0.75f,-0.75f, 0.75f, 0.75f };
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(wx[i], 0.22f, wz[i]);
        // Roata sta vertical pe axa X (torus in planul XY, deci rotit 90 pe Y)
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glutSolidTorus(0.13f, 0.22f, 8, 12);
        glPopMatrix();
    }
}

void carDraw() {
    glPushMatrix();
    glTranslatef(carX, 0.0f, carZ);
    glRotatef(carAngle * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    drawCarBody(0.9f, 0.1f, 0.1f);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void carDrawShadow() {
    float lx, ly, lz;
    lightingGetSunPos(lx, ly, lz);
    float plane[] = { 0,1,0,0 };
    float light[] = { lx, ly, lz, 1.0f };
    float sm[16];
    lightingMakeShadowMatrix(plane, light, sm);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glMultMatrixf(sm);
    glTranslatef(carX, 0.05f, carZ);
    glRotatef(carAngle * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
    glPushMatrix(); glTranslatef(0, 0.45f, 0); glScalef(1.8f, 0.7f, 2.4f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0, 1.0f, -0.1f); glScalef(1.5f, 0.6f, 1.4f); glutSolidCube(1.0f); glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

float carGetX() { return carX; }
float carGetZ() { return carZ; }