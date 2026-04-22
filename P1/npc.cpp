#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include "npc.h"
#include "lighting.h"

#ifndef M_PI
#define M_PI 3.14159265f
#endif

struct NPC { float x, z, angle, speed, timer, changeInterval, r, g, b; };
#define NUM_NPCS 5
static NPC npcs[NUM_NPCS];

struct PathCar { float x, z, angle, speed, r, g, b; int targetWP; };
static float waypoints[][2] = {
    {-25,-17.5f}, {0,-17.5f}, {25,-17.5f},
    {25, 0},      {25, 17.5f},
    {0, 17.5f},   {-25,17.5f},
    {-25, 0}
};
static int numWP = 8;
#define NUM_PATH_CARS 3
static PathCar pathCars[NUM_PATH_CARS];

void npcInit() {
    srand(42);
    for (int i = 0; i < NUM_NPCS; i++) {
        npcs[i].x = (float)(rand() % 30) - 15.0f;
        npcs[i].z = (float)(rand() % 20) - 10.0f;
        npcs[i].angle = (float)(rand() % 360) * M_PI / 180.0f;
        npcs[i].speed = 0.04f + (rand() % 5) * 0.01f;
        npcs[i].timer = 0.0f;
        npcs[i].changeInterval = 2.0f + (float)(rand() % 3);
        npcs[i].r = 0.3f + (rand() % 7) * 0.1f;
        npcs[i].g = 0.3f + (rand() % 7) * 0.1f;
        npcs[i].b = 0.3f + (rand() % 7) * 0.1f;
    }
    float col[3][3] = { {0.1f,0.4f,0.9f},{0.9f,0.6f,0.1f},{0.1f,0.8f,0.3f} };
    for (int i = 0; i < NUM_PATH_CARS; i++) {
        pathCars[i].targetWP = (i * numWP / NUM_PATH_CARS) % numWP;
        int wp = pathCars[i].targetWP;
        pathCars[i].x = waypoints[wp][0];
        pathCars[i].z = waypoints[wp][1];
        pathCars[i].angle = 0.0f;
        pathCars[i].speed = 0.15f + i * 0.05f;
        pathCars[i].r = col[i][0]; pathCars[i].g = col[i][1]; pathCars[i].b = col[i][2];
    }
}

void npcUpdate(float dt) {
    for (int i = 0; i < NUM_NPCS; i++) {
        npcs[i].timer += dt;
        if (npcs[i].timer > npcs[i].changeInterval) {
            npcs[i].angle = (float)(rand() % 360) * M_PI / 180.0f;
            npcs[i].timer = 0.0f;
        }
        npcs[i].x += sinf(npcs[i].angle) * npcs[i].speed;
        npcs[i].z += cosf(npcs[i].angle) * npcs[i].speed;
        if (npcs[i].x > 18.0f) { npcs[i].x = 18.0f; npcs[i].angle += M_PI; }
        if (npcs[i].x < -18.0f) { npcs[i].x = -18.0f; npcs[i].angle += M_PI; }
        if (npcs[i].z > 12.0f) { npcs[i].z = 12.0f; npcs[i].angle += M_PI; }
        if (npcs[i].z < -12.0f) { npcs[i].z = -12.0f; npcs[i].angle += M_PI; }
    }
    for (int i = 0; i < NUM_PATH_CARS; i++) {
        float tx = waypoints[pathCars[i].targetWP][0];
        float tz = waypoints[pathCars[i].targetWP][1];
        float dx = tx - pathCars[i].x;
        float dz = tz - pathCars[i].z;
        float dist = sqrtf(dx * dx + dz * dz);
        if (dist < 0.8f) {
            pathCars[i].targetWP = (pathCars[i].targetWP + 1) % numWP;
        }
        else {
            // atan2(dx,dz) = unghi fata de axa Z, matching carSpecialKeys
            pathCars[i].angle = atan2f(dx, dz);
            pathCars[i].x += (dx / dist) * pathCars[i].speed;
            pathCars[i].z += (dz / dist) * pathCars[i].speed;
        }
    }
}

static void drawPedestrian(float x, float z, float angle, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, 0, z);
    glRotatef(angle * 180.0f / M_PI, 0, 1, 0);
    glDisable(GL_TEXTURE_2D);
    glColor3f(r, g, b);
    glPushMatrix(); glTranslatef(0, 0.6f, 0); glScalef(0.5f, 0.8f, 0.3f); glutSolidCube(1); glPopMatrix();
    glColor3f(0.9f, 0.75f, 0.6f);
    glPushMatrix(); glTranslatef(0, 1.3f, 0); glutSolidSphere(0.22f, 8, 8); glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void drawNPCCar(float x, float z, float angle, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(angle * 180.0f / M_PI, 0, 1, 0);
    glDisable(GL_TEXTURE_2D);
    // Corp
    glColor3f(r, g, b);
    glPushMatrix(); glTranslatef(0, 0.45f, 0); glScalef(1.8f, 0.7f, 2.4f); glutSolidCube(1.0f); glPopMatrix();
    // Cabina
    glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
    glPushMatrix(); glTranslatef(0, 1.0f, -0.1f); glScalef(1.5f, 0.6f, 1.4f); glutSolidCube(1.0f); glPopMatrix();
    // Roti
    glColor3f(0.1f, 0.1f, 0.1f);
    float wx[] = { -1.0f, 1.0f,-1.0f, 1.0f };
    float wz[] = { -0.75f,-0.75f,0.75f,0.75f };
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(wx[i], 0.22f, wz[i]);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glutSolidTorus(0.13f, 0.22f, 8, 12);
        glPopMatrix();
    }
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void npcDraw() {
    for (int i = 0; i < NUM_NPCS; i++)
        drawPedestrian(npcs[i].x, npcs[i].z, npcs[i].angle, npcs[i].r, npcs[i].g, npcs[i].b);
    for (int i = 0; i < NUM_PATH_CARS; i++)
        drawNPCCar(pathCars[i].x, pathCars[i].z, pathCars[i].angle, pathCars[i].r, pathCars[i].g, pathCars[i].b);
}

void npcDrawShadows() {
    float lx, ly, lz;
    lightingGetSunPos(lx, ly, lz);
    float plane[] = { 0,1,0,0 }, light[] = { lx,ly,lz,1.0f }, sm[16];
    lightingMakeShadowMatrix(plane, light, sm);
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.45f);
    for (int i = 0; i < NUM_NPCS; i++) {
        glPushMatrix(); glMultMatrixf(sm); glTranslatef(npcs[i].x, 0.05f, npcs[i].z);
        glScalef(0.5f, 1.5f, 0.3f); glutSolidCube(1); glPopMatrix();
    }
    for (int i = 0; i < NUM_PATH_CARS; i++) {
        glPushMatrix(); glMultMatrixf(sm);
        glTranslatef(pathCars[i].x, 0.05f, pathCars[i].z);
        glRotatef(pathCars[i].angle * 180.0f / M_PI, 0, 1, 0);
        glPushMatrix(); glTranslatef(0, 0.45f, 0); glScalef(1.8f, 0.7f, 2.4f); glutSolidCube(1); glPopMatrix();
        glPopMatrix();
    }
    glDisable(GL_BLEND); glEnable(GL_LIGHTING); glEnable(GL_TEXTURE_2D);
}