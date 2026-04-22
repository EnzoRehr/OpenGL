#include <GL/freeglut.h>
#include <cmath>
#include "scene.h"
#include "lighting.h"

#ifndef M_PI
#define M_PI 3.14159265f
#endif

static GLuint texGrass = 0, texSky = 0, texRoad = 0, texWall = 0, texRoof = 0;

#define GRID_SIZE 50
#define CELL_SIZE 2.2f
static float heightMap[GRID_SIZE][GRID_SIZE];

static GLuint makeSolidTexture(int r, int g, int b) {
    const int SZ = 64;
    unsigned char data[SZ * SZ * 3];
    for (int i = 0; i < SZ; i++)
        for (int j = 0; j < SZ; j++) {
            float var = (float)(((i * 3 + j * 7) % 20) - 10);
            int idx = (i * SZ + j) * 3;
            data[idx + 0] = (unsigned char)fmaxf(0, fminf(255, (float)r + var));
            data[idx + 1] = (unsigned char)fmaxf(0, fminf(255, (float)g + var / 2.0f));
            data[idx + 2] = (unsigned char)fmaxf(0, fminf(255, (float)b + var / 3.0f));
        }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, SZ, SZ, GL_RGB, GL_UNSIGNED_BYTE, data);
    return id;
}

void sceneInit() {
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    texGrass = makeSolidTexture(45, 120, 35);
    texSky = makeSolidTexture(90, 150, 210);
    texRoad = makeSolidTexture(65, 65, 65);
    texWall = makeSolidTexture(185, 155, 120);
    texRoof = makeSolidTexture(155, 45, 45);

    float offset = GRID_SIZE * CELL_SIZE / 2.0f;
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++) {
            float wx = i * CELL_SIZE - offset;
            float wz = j * CELL_SIZE - offset;
            float h = 3.0f * sinf(wx * 0.05f) * cosf(wz * 0.05f)
                + 1.5f * sinf(wx * 0.08f)
                + 0.8f * cosf(wz * 0.12f);
            float dist = sqrtf(wx * wx + wz * wz);
            // Zona plata: r<38 = 0, r=38..60 = tranzitie lina
            if (dist < 38.0f)
                h = 0.0f;
            else if (dist < 60.0f) {
                float t = (dist - 38.0f) / 22.0f;
                h *= t * t * t;
            }
            heightMap[i][j] = h;
        }
}

void sceneDrawGround() {
    // Sol plat la Y=0
    glBindTexture(GL_TEXTURE_2D, texGrass);
    glColor3f(1, 1, 1);
    // Polygon offset pentru a evita z-fighting cu terenul
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 2.0f);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-60, 0, -60);
    glTexCoord2f(12, 0); glVertex3f(60, 0, -60);
    glTexCoord2f(12, 12); glVertex3f(60, 0, 60);
    glTexCoord2f(0, 12); glVertex3f(-60, 0, 60);
    glEnd();
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void sceneDrawSkybox() {
    float S = 200.0f;
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texSky); glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-S, -10, -S); glTexCoord2f(1, 0); glVertex3f(S, -10, -S);
    glTexCoord2f(1, 1); glVertex3f(S, S, -S); glTexCoord2f(0, 1); glVertex3f(-S, S, -S);
    glEnd();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(S, -10, S); glTexCoord2f(1, 0); glVertex3f(-S, -10, S);
    glTexCoord2f(1, 1); glVertex3f(-S, S, S); glTexCoord2f(0, 1); glVertex3f(S, S, S);
    glEnd();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-S, -10, S); glTexCoord2f(1, 0); glVertex3f(-S, -10, -S);
    glTexCoord2f(1, 1); glVertex3f(-S, S, -S); glTexCoord2f(0, 1); glVertex3f(-S, S, S);
    glEnd();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(S, -10, -S); glTexCoord2f(1, 0); glVertex3f(S, -10, S);
    glTexCoord2f(1, 1); glVertex3f(S, S, S); glTexCoord2f(0, 1); glVertex3f(S, S, -S);
    glEnd();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-S, S, -S); glTexCoord2f(1, 0); glVertex3f(S, S, -S);
    glTexCoord2f(1, 1); glVertex3f(S, S, S); glTexCoord2f(0, 1); glVertex3f(-S, S, S);
    glEnd();
    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING);
}

static void calcNormal(float ax, float ay, float az,
    float bx, float by, float bz,
    float cx, float cy, float cz) {
    float ux = bx - ax, uy = by - ay, uz = bz - az;
    float vx = cx - ax, vy = cy - ay, vz = cz - az;
    float nx = uy * vz - uz * vy, ny = uz * vx - ux * vz, nz = ux * vy - uy * vx;
    float len = sqrtf(nx * nx + ny * ny + nz * nz);
    if (len > 0)glNormal3f(nx / len, ny / len, nz / len);
}

void sceneDrawTerrain() {
    glBindTexture(GL_TEXTURE_2D, texGrass);
    glColor3f(1, 1, 1);
    float offset = GRID_SIZE * CELL_SIZE / 2.0f;
    for (int i = 0; i < GRID_SIZE - 1; i++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < GRID_SIZE; j++) {
            float x0 = i * CELL_SIZE - offset, x1 = (i + 1) * CELL_SIZE - offset;
            float z = j * CELL_SIZE - offset;
            float h00 = heightMap[i][j], h10 = heightMap[i + 1][j];
            float hN = (j + 1 < GRID_SIZE) ? heightMap[i][j + 1] : h00;
            calcNormal(x0, h00, z, x1, h10, z, x0, hN, z + CELL_SIZE);
            glTexCoord2f((float)i / GRID_SIZE * 6, (float)j / GRID_SIZE * 6);
            glVertex3f(x0, h00, z);
            glTexCoord2f((float)(i + 1) / GRID_SIZE * 6, (float)j / GRID_SIZE * 6);
            glVertex3f(x1, h10, z);
        }
        glEnd();
    }
}

void sceneDrawRoad() {
    glBindTexture(GL_TEXTURE_2D, texRoad);
    glColor3f(1, 1, 1);
    // Road la Y=0.05 - suficient deasupra solului fara z-fighting
    float y = 0.05f;
    glBegin(GL_QUADS); glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-30, y, -22); glTexCoord2f(6, 0); glVertex3f(30, y, -22);
    glTexCoord2f(6, 1); glVertex3f(30, y, -13);  glTexCoord2f(0, 1); glVertex3f(-30, y, -13);
    glEnd();
    glBegin(GL_QUADS); glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-30, y, 13); glTexCoord2f(6, 0); glVertex3f(30, y, 13);
    glTexCoord2f(6, 1); glVertex3f(30, y, 22);  glTexCoord2f(0, 1); glVertex3f(-30, y, 22);
    glEnd();
    glBegin(GL_QUADS); glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-30, y, -13); glTexCoord2f(0, 4); glVertex3f(-30, y, 13);
    glTexCoord2f(1, 4); glVertex3f(-20, y, 13);  glTexCoord2f(1, 0); glVertex3f(-20, y, -13);
    glEnd();
    glBegin(GL_QUADS); glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(20, y, -13); glTexCoord2f(0, 4); glVertex3f(20, y, 13);
    glTexCoord2f(1, 4); glVertex3f(30, y, 13);  glTexCoord2f(1, 0); glVertex3f(30, y, -13);
    glEnd();
}

static void drawBuilding(float x, float z, float w, float h, float d) {
    glPushMatrix(); glTranslatef(x, 0, z);
    glBindTexture(GL_TEXTURE_2D, texWall); glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-w / 2, 0, d / 2); glTexCoord2f(1, 0); glVertex3f(w / 2, 0, d / 2);
    glTexCoord2f(1, 1); glVertex3f(w / 2, h, d / 2);  glTexCoord2f(0, 1); glVertex3f(-w / 2, h, d / 2);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(w / 2, 0, -d / 2); glTexCoord2f(1, 0); glVertex3f(-w / 2, 0, -d / 2);
    glTexCoord2f(1, 1); glVertex3f(-w / 2, h, -d / 2); glTexCoord2f(0, 1); glVertex3f(w / 2, h, -d / 2);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-w / 2, 0, -d / 2); glTexCoord2f(1, 0); glVertex3f(-w / 2, 0, d / 2);
    glTexCoord2f(1, 1); glVertex3f(-w / 2, h, d / 2); glTexCoord2f(0, 1); glVertex3f(-w / 2, h, -d / 2);
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(w / 2, 0, d / 2);  glTexCoord2f(1, 0); glVertex3f(w / 2, 0, -d / 2);
    glTexCoord2f(1, 1); glVertex3f(w / 2, h, -d / 2); glTexCoord2f(0, 1); glVertex3f(w / 2, h, d / 2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, texRoof);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-w / 2, h, -d / 2); glTexCoord2f(1, 0); glVertex3f(w / 2, h, -d / 2);
    glTexCoord2f(1, 1); glVertex3f(w / 2, h, d / 2);   glTexCoord2f(0, 1); glVertex3f(-w / 2, h, d / 2);
    glEnd();
    glPopMatrix();
}

static void drawTreeGeometry() {
    glColor3f(0.45f, 0.24f, 0.08f);
    glPushMatrix(); glTranslatef(0, 1.25f, 0); glScalef(0.35f, 2.5f, 0.35f); glutSolidCube(1); glPopMatrix();
    glColor3f(0.12f, 0.52f, 0.12f);
    glPushMatrix(); glTranslatef(0, 2.8f, 0); glScalef(2.8f, 1.8f, 2.8f); glutSolidCube(1); glPopMatrix();
    glColor3f(0.10f, 0.58f, 0.10f);
    glPushMatrix(); glTranslatef(0, 4.2f, 0); glScalef(2.0f, 1.6f, 2.0f); glutSolidCube(1); glPopMatrix();
    glColor3f(0.08f, 0.62f, 0.08f);
    glPushMatrix(); glTranslatef(0, 5.4f, 0); glScalef(1.2f, 1.4f, 1.2f); glutSolidCube(1); glPopMatrix();
}

static void drawTreeShadowEllipse(float cx, float cz, float lx, float ly, float lz) {
    float offX = -(lx / ly) * 5.0f;
    float offZ = -(lz / ly) * 5.0f;
    float y = 0.06f; // deasupra road-ului
    int seg = 24;
    float rX = 2.2f, rZ = 2.2f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx + offX * 0.4f, y, cz + offZ * 0.4f);
    for (int k = 0; k <= seg; k++) {
        float a = 2.0f * (float)M_PI * k / seg;
        glVertex3f(cx + offX * 0.4f + rX * cosf(a) + offX * 0.25f, y,
            cz + offZ * 0.4f + rZ * sinf(a) + offZ * 0.25f);
    }
    glEnd();
}

void sceneDrawObjects() {
    drawBuilding(-40, 0, 6, 12, 6);
    drawBuilding(-40, 15, 5, 9, 5);
    drawBuilding(-40, -15, 7, 15, 6);
    drawBuilding(40, 0, 6, 10, 6);
    drawBuilding(40, 15, 5, 8, 5);
    drawBuilding(40, -15, 7, 14, 7);

    float treeX[] = { -35,-35,-35,35,35,35,-8, 8 };
    float treeZ[] = { -8,  0,  8,-8, 0, 8,-28,-28 };

    // Umbre copaci - elipse negre
    float lx, ly, lz;
    lightingGetSunPos(lx, ly, lz);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.5f);
    glNormal3f(0, 1, 0);
    for (int i = 0; i < 8; i++)
        drawTreeShadowEllipse(treeX[i], treeZ[i], lx, ly, lz);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Copaci
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glTranslatef(treeX[i], 0, treeZ[i]);
        glDisable(GL_TEXTURE_2D);
        drawTreeGeometry();
        glEnable(GL_TEXTURE_2D);
        glPopMatrix();
    }
}