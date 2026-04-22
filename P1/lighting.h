#pragma once

void lightingInit();
void lightingApply();
void lightingDrawPoles();
void lightingGetSunPos(float& x, float& y, float& z);
void lightingMakeShadowMatrix(float ground[4], float light[4], float mat[16]);
