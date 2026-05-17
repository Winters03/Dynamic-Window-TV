#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/tracking.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/videoio.hpp>


#include "config.h"
#include "variables.h"
#include "functions.h"
#include "headtracking.h"

using namespace std;
using namespace std::chrono;


int main() {
    cout << "Starting Dynamic Window TV..." << endl;

    /////////////       Make Variables      /////////////
    cv::Mat camera;
    cv::Mat background_SOURCE;
    cv::Mat background;
    cv::Mat cropped = cv::Mat::zeros(1920, 1080, CV_8UC3);
    cv::VideoCapture background_Video;
    HeadLocations headData;
    Window windowData;
    windowConfig config;

    getData(config);


    /////////////       Loading      /////////////
    cv::dnn::Net net = cv::dnn::readNetFromCaffe(
        "./models/deploy.prototxt",
        "./models/res10_300x300_ssd_iter_140000.caffemodel"
    );

    cv::VideoCapture video(1);
    if (!video.isOpened()) {
        cout << "External Webcam Not Connected." << endl << "Using Laptop Camera" << endl;
        video.open(0);
    }

    video.set(cv::CAP_PROP_BUFFERSIZE, 1);
    video.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    video.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    //video.set(cv::CAP_PROP_FPS, 1);
    video.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
    //video.set(cv::CAP_PROP_EXPOSURE, -50);
    //video.set(cv::CAP_PROP_GAIN, -10);

    cv::Ptr<cv::FaceDetectorYN> detector = cv::FaceDetectorYN::create(
        "./models/face_detection_yunet_2023mar.onnx", "", cv::Size(video.get(cv::CAP_PROP_FRAME_WIDTH),video.get(cv::CAP_PROP_FRAME_HEIGHT))
    );

    cv::Ptr<cv::FaceDetectorYN> detector_2 = cv::FaceDetectorYN::create("./models/face_detection_yunet_2023mar.onnx", "", cv::Size(1920, 1080), 0.6f, 0.3f, 5000);

    if (config.bgType == "mp4") {
        background_Video.open("./backgrounds/"+config.bgName+".mp4");

        if(!background_Video.isOpened()){
            std::cout << "Error opening video stream" << std::endl;
            return -1;
        }
    }
    else {
        background_SOURCE = cv::imread("./backgrounds/"+config.bgName+"."+config.bgType, cv::IMREAD_ANYDEPTH | cv::IMREAD_COLOR);
        cv::Ptr<cv::TonemapReinhard> tonemap = cv::createTonemapReinhard(config.gamma_Day, 0.0f, 0.0f, 0.0f);
        tonemap->process(background_SOURCE, background_SOURCE);
        background_SOURCE.convertTo(background_SOURCE, CV_8UC3, 255.0);

        cv::resize(background_SOURCE, background_SOURCE, cv::Size(7680, 4320), 0, 0, cv::INTER_LANCZOS4);

    }

    cv::namedWindow("Cropped", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Cropped", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

    auto start = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto prevTime = start;
    /////////////       Looping      /////////////
    while (true) {
        currentTime = std::chrono::high_resolution_clock::now();

        video >> camera;
        
        if (config.bgType == "mp4") {
            background_Video >> background;
            //background_Video.read(background);
            cv::resize(background, background, cv::Size(7680, 4320), 0, 0, cv::INTER_LANCZOS4); // 7680 x 4320 is 8K UHD, the resolution of the background images. This ensures that the cropping and scaling are consistent across all backgrounds, regardless of their original resolution.
        }
        else {
            background = background_SOURCE.clone();
        }
        
        getLocationOfHead_res10(headData, camera, net);
        //getLocationOfHead_YuNet(headData, camera, detector);
        //getLocationOfHead_YuNet_2(headData, camera, detector_2);
        Smoothing(headData, 0.4, 0.5);
        getLocationOfCropped(windowData, headData, background.cols, background.rows, camera.cols, camera.rows, config.movementScale_X, config.movementScale_Y);
        setCropping(windowData, background.rows, background.cols, WindowDimensions_X, WindowDimensions_Y, config.scale);

        


        //cropped = background(cv::Rect(windowData.LX, windowData.TY, WindowDimensions_X*config.scale, WindowDimensions_Y*config.scale)).clone();
        if (config.bgType == "mp4") {
            cropped = background(cv::Rect(windowData.LX, windowData.TY, WindowDimensions_X*config.scale, WindowDimensions_Y*config.scale));
        }
        else {
            cropped = background_SOURCE(cv::Rect(windowData.LX, windowData.TY, WindowDimensions_X*config.scale, WindowDimensions_Y*config.scale));
        }

        cv::circle(camera, {headData.X_Pixel_Target, headData.Y_Pixel_Target}, 6, {0, 255, 0}, -1);
        cv::circle(camera, {headData.X_Pixel_Current, headData.Y_Pixel_Current}, 4, {0, 0, 255}, -1);
        cv::circle(background, {windowData.X_Location, windowData.Y_Location}, 6, {255, 0, 0}, -1);
        cv::rectangle(background, {windowData.LX, windowData.TY}, {windowData.RX,windowData.BY}, {255, 0, 0}, 6, -1);


        cv::imshow("Head Tracking", camera);
        //cv::imshow("Background", background);
        cv::imshow("Cropped", cropped);

        if (cv::waitKey(1) == 'q') break;

        chrono::duration<double, std::milli> elapsed = currentTime - prevTime;
        //cout << 1000/elapsed.count() << endl;
        prevTime = currentTime;
    }

    return 0;
}