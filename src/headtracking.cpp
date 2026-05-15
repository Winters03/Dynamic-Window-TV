#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include "variables.h"

#include <iostream>
using namespace std;

void getLocationOfHead(HeadLocations& headData, cv::Mat& frame, cv::dnn::Net& net) {

    int h = frame.rows;
    int w = frame.cols;

    // Preprocess
    cv::Mat blob = cv::dnn::blobFromImage(
        frame, 1.0, cv::Size(300, 300),
        cv::Scalar(104, 177, 123)
    );

    net.setInput(blob);
    cv::Mat detections = net.forward();


    float* data = (float*)detections.data;

    for (int i = 0; i < detections.size[2]; i++) {
        float confidence = data[i * 7 + 2];

        if (confidence > 0.6) {
            int x1 = int(data[i * 7 + 3] * w);
            int y1 = int(data[i * 7 + 4] * h);
            int x2 = int(data[i * 7 + 5] * w);
            int y2 = int(data[i * 7 + 6] * h);

            // 🎯 HEAD POSITION (CENTER PIXEL)
            headData.X_Pixel_Target = (x1 + x2) / 2;
            headData.Y_Pixel_Target = (y1 + y2) / 2;
        }
    }
}


void Smoothing(HeadLocations& headData, float smoothing) {

    
    headData.X_Pixel_Current = (headData.X_Pixel_Target*smoothing) + (headData.X_Pixel_Current*(1-smoothing));
    headData.Y_Pixel_Current = (headData.Y_Pixel_Target*smoothing) + (headData.Y_Pixel_Current*(1-smoothing));
}