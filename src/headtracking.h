#ifndef HEADTRACKING_H
#define HEADTRACKING_H

void getLocationOfHead_res10(HeadLocations& headData, cv::Mat& frame, cv::dnn::Net& net);
void getLocationOfHead_YuNet(HeadLocations& headData, cv::Mat& frame, cv::Ptr<cv::FaceDetectorYN>& net);
void getLocationOfHead_YuNet_2(HeadLocations& headData, cv::Mat& frame, cv::Ptr<cv::FaceDetectorYN> net);
void Smoothing(HeadLocations& headData, float smoothing_X, float smoothing_Y);

#endif