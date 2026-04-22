#include <GL/freeglut.h>
#include <cstdio>
#include "camera.h"
#include "lighting.h"
#include "scene.h"
#include "car.h"
#include "npc.h"

int windowWidth = 1024;
int windowHeight = 768;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    cameraApply();
    lightingApply();

    sceneDrawSkybox();
    sceneDrawGround();
    sceneDrawTerrain();
    sceneDrawRoad();
    sceneDrawObjects();
    lightingDrawPoles();

    carDrawShadow();
    npcDrawShadows();
    carDraw();
    npcDraw();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = (h == 0) ? 1 : h;
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    cameraKeyboard(key);
    if (key == 27) exit(0);
    glutPostRedisplay();
}

// Sagetile controleaza masina
void specialKeys(int key, int x, int y) {
    carSpecialKeys(key);
    glutPostRedisplay();
}

// Taste speciale apasate (held down) - pentru miscare continua
void specialKeysUp(int key, int x, int y) {
    glutPostRedisplay();
}

int lastMouseX = -1, lastMouseY = -1;
void mouseMotion(int x, int y) {
    if (lastMouseX >= 0) {
        float dx = (x - lastMouseX) * 0.005f;
        float dy = (y - lastMouseY) * 0.005f;
        cameraMouseMove(dx, dy);
    }
    lastMouseX = x; lastMouseY = y;
    glutPostRedisplay();
}
void mouseButton(int button, int state, int x, int y) {
    if (state == GLUT_UP) { lastMouseX = -1; lastMouseY = -1; }
}

void timer(int value) {
    float dt = 0.016f;
    npcUpdate(dt);
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Proiect OpenGL");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    sceneInit();
    lightingInit();
    cameraInit();
    carInit();
    npcInit();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);
    glutTimerFunc(16, timer, 0);

    printf("W/S/A/D/Q/E = Camera | Sageti = Masina | Mouse drag = Rotatie\n");

    glutMainLoop();
    return 0;
}