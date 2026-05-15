#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>

struct windowConfig {
    std::string bgName;
    std::string bgType;
    float gamma_Day;
    float gamma_TwilightNight;
    float gamma_TwilightDay;
    float scale;
    float movementScale_X;
    float movementScale_Y;
};

struct Window {
    int X_Location;
    int Y_Location;
    int LX;
    int RX;
    int TY;
    int BY;
};

struct HeadLocations {
    int X_Pixel_Target;
    int Y_Pixel_Target;
    int X_Pixel_Current;
    int Y_Pixel_Current;
};


#endif