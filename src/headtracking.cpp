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
    float confid = 0;

    for (int i = 0; i < detections.size[2]; i++) {
        float confidence = data[i * 7 + 2];

        if (confidence > confid) {
            confid = confidence;
        }


        if (confidence > 0.25) {
            int x1 = int(data[i * 7 + 3] * w);
            int y1 = int(data[i * 7 + 4] * h);
            int x2 = int(data[i * 7 + 5] * w);
            int y2 = int(data[i * 7 + 6] * h);

            // 🎯 HEAD POSITION (CENTER PIXEL)
            headData.X_Pixel_Target = (x1 + x2) / 2;
            headData.Y_Pixel_Target = (y1 + y2) / 2;
        }
    }
    cv::putText(frame, "Confidence: " + std::to_string(confid), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 1, cv::LINE_AA);
}


void Smoothing(HeadLocations& headData, float smoothing_X, float smoothing_Y) {

    
    headData.X_Pixel_Current = (headData.X_Pixel_Target*smoothing_X) + (headData.X_Pixel_Current*(1-smoothing_X));
    headData.Y_Pixel_Current = (headData.Y_Pixel_Target*smoothing_Y) + (headData.Y_Pixel_Current*(1-smoothing_Y));
}