#include <GL/freeglut.h>
#include <cmath>
#include "camera.h"

// Pozitia camerei in lume
static float camX     =  0.0f;
static float camY     =  8.0f;
static float camZ     = 40.0f;

// Unghiuri de rotatie (yaw = stanga/dreapta, pitch = sus/jos)
static float camYaw   = 0.0f;
static float camPitch = -0.15f;

static float camSpeed = 0.5f;

void cameraInit() {
    camX = 0.0f; camY = 8.0f; camZ = 40.0f;
    camYaw = 0.0f; camPitch = -0.15f;
}

void cameraApply() {
    // Calculeaza directia de privire din unghiuri
    float lookX = camX + sinf(camYaw) * cosf(camPitch);
    float lookY = camY + sinf(camPitch);
    float lookZ = camZ - cosf(camYaw) * cosf(camPitch);

    gluLookAt(
        camX, camY, camZ,     // pozitia ochiului
        lookX, lookY, lookZ,  // punctul spre care privim
        0.0f,  1.0f,  0.0f   // vectorul "sus"
    );
}

void cameraKeyboard(unsigned char key) {
    switch (key) {
        // Miscare inainte/inapoi pe directia de privire
        case 'w': case 'W':
            camX += sinf(camYaw) * camSpeed;
            camZ -= cosf(camYaw) * camSpeed;
            break;
        case 's': case 'S':
            camX -= sinf(camYaw) * camSpeed;
            camZ += cosf(camYaw) * camSpeed;
            break;
        // Rotatie stanga/dreapta
        case 'a': case 'A':
            camYaw -= 0.05f;
            break;
        case 'd': case 'D':
            camYaw += 0.05f;
            break;
        // Miscare sus/jos
        case 'q': case 'Q':
            camY += camSpeed;
            break;
        case 'e': case 'E':
            camY -= camSpeed;
            break;
    }
}

void cameraMouseMove(float dx, float dy) {
    camYaw   += dx;
    camPitch -= dy;
    // Limiteaza pitch-ul sa nu se rastoane camera
    if (camPitch >  1.5f) camPitch =  1.5f;
    if (camPitch < -1.5f) camPitch = -1.5f;
}
