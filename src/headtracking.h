#ifndef HEADTRACKING_H
#define HEADTRACKING_H

void getLocationOfHead(HeadLocations& headData, cv::Mat& frame, cv::dnn::Net& net);
void Smoothing(HeadLocations& headData, float smoothing_X, float smoothing_Y);

#endif