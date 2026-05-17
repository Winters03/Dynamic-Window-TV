#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/tracking.hpp>
#include "variables.h"

#include <iostream>
using namespace std;


void applyGammaCorrection(cv::Mat& frame, double gamma) {
    // Create a lookup table for speed (doing math on 256 values instead of every pixel)
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    
    for (int i = 0; i < 256; ++i) {
        // Formula: ((Old_Pixel / 255) ^ gamma) * 255
        p[i] = cv::saturate_cast<uchar>(std::pow(i / 255.0, gamma) * 255.0);
    }
    
    // Instantly map the entire frame to the new glare-reduced curve
    cv::LUT(frame, lookUpTable, frame);
}

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


void getLocationOfHead_res10(HeadLocations& headData, cv::Mat& frame, cv::dnn::Net& net) {

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
    cv::putText(frame, "Confidence: " + std::to_string(confid), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 1, cv::LINE_AA);
}

void getLocationOfHead_YuNet(HeadLocations& headData, cv::Mat& frame, cv::Ptr<cv::FaceDetectorYN>& net) {
    
    //applyGammaCorrection(frame, 2.0); // Adjust gamma to reduce glare

    normalizeLighting(frame);

    float highest_confidence = 0;
    
    cv::Mat faces;
    net->detect(frame, faces);

    // Loop through detections
    for (int i = 0; i < faces.rows; i++) {
        float confidence = faces.at<float>(i, 14); // Confidence is at index 14



        if (confidence > highest_confidence) {
            highest_confidence = confidence;
        }

        if (confidence > 0.6) {
            float x = faces.at<float>(i, 0);
            float y = faces.at<float>(i, 1);
            float w = faces.at<float>(i, 2);
            float h = faces.at<float>(i, 3);
            
            // Calculate center pixel target
            headData.X_Pixel_Target = x + (w / 2);
            headData.Y_Pixel_Target = y + (h / 2);
        }
    }

    cv::putText(frame, "Confidence: " + std::to_string(highest_confidence), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 1, cv::LINE_AA);
}

void getLocationOfHead_YuNet_2(HeadLocations& headData, cv::Mat& frame, cv::Ptr<cv::FaceDetectorYN> net) {
    cv::Mat faces;

    cv::Ptr<cv::TrackerCSRT> tracker;
    bool tracking = false;

    cv::Rect trackedFace;
    net->setInputSize(frame.size());

    net->detect(frame, faces);
    //cout << faces << endl;

    if (faces.rows > 0) {
        float* data = faces.ptr<float>(0);

        trackedFace = cv::Rect(
            int(data[0]),
            int(data[1]),
            int(data[2]),
            int(data[3])
        );

        tracker = cv::TrackerCSRT::create();
        tracker->init(frame, trackedFace);
        tracking = true;
    }
    if (tracking == true) {

        headData.X_Pixel_Target = trackedFace.x + (trackedFace.width / 2);
        headData.Y_Pixel_Target = trackedFace.y + (trackedFace.height / 2);
    }

}

void Smoothing(HeadLocations& headData, float smoothing_X, float smoothing_Y) {

    
    headData.X_Pixel_Current = (headData.X_Pixel_Target*smoothing_X) + (headData.X_Pixel_Current*(1-smoothing_X));
    headData.Y_Pixel_Current = (headData.Y_Pixel_Target*smoothing_Y) + (headData.Y_Pixel_Current*(1-smoothing_Y));
}