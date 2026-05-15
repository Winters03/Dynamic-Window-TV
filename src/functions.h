#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "variables.h"

void getData(windowConfig& config);
void getLocationOfCropped(Window& Window, HeadLocations& HeadLocations, int background_X, int background_Y, int camera_X, int camera_Y, float movementScale_X, float movementScale_Y);
void setCropping(Window& Window, int background_X, int background_Y, int CropSize_X, int CropSize_Y, float scale);

#endif