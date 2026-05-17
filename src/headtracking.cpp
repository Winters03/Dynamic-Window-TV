#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking_legacy.hpp>
#include "variables.h"

#include <iostream>
using namespace std;

void normalizeLighting(cv::Mat& frame) {
    cv::Mat lab_image;
    
    // 1. Convert from BGR to Lab color space. 
    // This separates lightness (L) from color (a, b) so we don't distort your skin tone.
    cv::cvtColor(frame, lab_image, cv::COLOR_BGR2Lab);

    // 2. Split the channels
    std::vector<cv::Mat> lab_planes(3);
    cv::split(lab_image, lab_planes);

    // 3. Create and apply CLAHE to the Lightness channel (Index 0)
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(3.0);       // Controls how aggressively it boosts contrast (2.0 - 4.0 is typical)
    clahe->setTilesGridSize(cv::Size(8, 8)); // Break the image into an 8x8 grid for localized adjustments
    clahe->apply(lab_planes[0], lab_planes[0]);

    // 4. Merge the adjusted lightness channel back with the color channels
    cv::merge(lab_planes, lab_image);

    // 5. Convert back to standard BGR for your neural network
    cv::cvtColor(lab_image, frame, cv::COLOR_Lab2BGR);
}


void getLocationOfHead(HeadLocations& headData,cv::HOGDescriptor& hog, cv::Mat& frame, cv::dnn::Net& net) {

    normalizeLighting(frame);


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

    std::vector<cv::Rect> found;
    // Resize or downscale frame here if detection is slow
    hog.detectMultiScale(frame, found, 0, cv::Size(8,8), cv::Size(32,32), 1.05, 2);

    if (found.empty()) {
        return;
    }

    cv::Rect roi = found[0];

    // 3. Create the ultra-fast MOSSE tracker
    cv::Ptr<cv::Tracker> tracker = cv::legacy::upgradeTrackingAPI(cv::legacy::TrackerMOSSE::create());
    tracker->init(frame, roi);

    bool success = tracker->update(frame, roi);

    cv::putText(frame, "Face Confidence  : " + std::to_string(confid), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 1, cv::LINE_AA);
    cv::putText(frame, "Person Confidence: " + std::to_string(confid), cv::Point(50, 100), cv::FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 1, cv::LINE_AA);
    if (success) {
        headData.X_Pixel_Target = roi.x + roi.width / 2;
        headData.Y_Pixel_Target = roi.y + roi.height / 2;
        cv::rectangle(frame, roi, cv::Scalar(0, 255, 0), 2);
    }


}


void Smoothing(HeadLocations& headData, float smoothing_X, float smoothing_Y) {

    
    headData.X_Pixel_Current = (headData.X_Pixel_Target*smoothing_X) + (headData.X_Pixel_Current*(1-smoothing_X));
    headData.Y_Pixel_Current = (headData.Y_Pixel_Target*smoothing_Y) + (headData.Y_Pixel_Current*(1-smoothing_Y));
}