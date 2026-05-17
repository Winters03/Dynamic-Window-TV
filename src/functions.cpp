#include "variables.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
using std::string;
using std::vector;


void getLocationOfCropped(Window& Window, HeadLocations& HeadLocations, int background_X, int background_Y, int camera_X, int camera_Y, float movementScale_X,float movementScale_Y) {
    Window.X_Location = (background_X/2) + ((HeadLocations.X_Pixel_Current - (camera_X/2))*movementScale_X);
    Window.Y_Location = (background_Y/2) - ((HeadLocations.Y_Pixel_Current - (camera_Y/2))*movementScale_Y);
}


void setCropping(Window& Window, int background_Y, int background_X, int CropSize_X, int CropSize_Y, float scale) {

    CropSize_X = CropSize_X* scale;
    CropSize_Y = CropSize_Y* scale;

    if ((Window.X_Location - CropSize_X/2) <= 0) {
        Window.LX = 0;
        Window.RX = CropSize_X;
    }
    else if ((Window.X_Location + CropSize_X/2) >= background_X) {
        Window.LX = background_X - CropSize_X;
        Window.RX = background_X;
    }
    else {
        Window.LX = Window.X_Location - CropSize_X/2;
        Window.RX = Window.X_Location + CropSize_X/2;
    }

    
    if ((Window.Y_Location - CropSize_Y/2) <= 0) {
        Window.TY = 0;
        Window.BY = CropSize_Y;
    }
    else if ((Window.Y_Location + CropSize_Y/2) >= background_Y) {
        Window.TY = background_Y - CropSize_Y;
        Window.BY = background_Y;
    }
    else {
        Window.TY = Window.Y_Location - CropSize_Y/2;
        Window.BY = Window.Y_Location + CropSize_Y/2;
    }
    
}


vector<vector<string>> catalogRead() {
    ifstream catalog("./catalog.csv");

    vector<vector<string>> data;
    string line;

    // Read the file line by line
    while (getline(catalog, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        // Split the line by commas
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);
    }

    catalog.close();

    data.erase(data.begin());

    return data;
}


void printVector(vector<vector<string>> data) {
    for (const auto& row : data) {
        for (const auto& cell : row) {
            std::cout << cell << "\t\t"; // Print individual string
        }
        std::cout << "\n"; // New line after each row
    }
}

vector<string> splitString(string STR, char dec) {
    vector<std::string> words;
    stringstream ss(STR);
    string word;

    while (getline(ss, word, dec)) {
        words.push_back(word);
    }
    return words;
}

void getData(windowConfig& config) {
    string file = "waterfront";

    vector<vector<string>> catalog = catalogRead();
    vector<string> bgDATA;
    //printVector(catalog);

    for (auto row : catalog) {
        if (row[0] == file) {
            bgDATA = row;
            break;
        }
    }

    //cout << bgDATA[0] << endl;

    config.bgName = bgDATA[0];
    config.bgType = bgDATA[1];

    if (config.bgType != "mp4") {
        config.gamma_Day = stof(bgDATA[3]);
        config.gamma_TwilightNight = stof(bgDATA[4]);
        config.gamma_TwilightDay = stof(bgDATA[5]);
    }

    //config.scale = stof(bgDATA[3]);
    config.movementScale_X = stof(splitString(bgDATA[2],'-')[0]);
    config.movementScale_Y = stof(splitString(bgDATA[2],'-')[1]);

}

/*
    std::string bgName;
    std::string bgType;
    float gamma_Day;
    float gamma_TwilightNight;
    float gamma_TwilightDay;
    float scale;
    float movementScale_X;
    float movementScale_Y;

*/