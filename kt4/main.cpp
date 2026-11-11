#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <opencv2/imgproc.hpp>

#include <opencv2/opencv.hpp>

int main(int argc, char** argv){
    cv::VideoCapture cap;
    if (argc > 1){
        cap.open(argv[1]);
    } else {
        cap.open(0);
    }

    if(!cap.isOpened()){
        std::cerr << "Ошибка.\n";
        return -1;
    }
    std::cout << "Запуск.\n"
            << "[Пробел] - переключение режима\n"
            << "[R] - поворот на 90 градусов\n"
            << "[ESC] - выход\n";
    
    cv::Mat frame;
    int displayMode = 0;
    bool applyRotation = false;

    cv::VideoWriter video;
    bool isRecording = false;

    while(true){
        cap >> frame;
        if(frame.empty()){
            break;
        }

        if(applyRotation){
            cv::Point2f center(frame.cols / 2.0f, frame.rows / 2.0f);
            double angle = 90.0;
            double scale = 1.0;
            cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, scale);
            cv::warpAffine(frame, frame, rotMat, frame.size());
        }
        cv::Mat lab;
        cv::cvtColor(frame, lab, cv::COLOR_BGR2Lab);

        std::vector<cv::Mat> labChannels;

        cv::split(lab, labChannels);
        cv::Mat channelL = labChannels[0];
        cv::Mat channelA = labChannels[1];
        cv::Mat channelB = labChannels[2];

        cv::Mat result;

        if (displayMode == 0){
            result =frame;
        }
        else if (displayMode == 1){
            result = channelL;
            
        }
        else if(displayMode == 2){
            cv::Mat modifiedA = channelA.clone();
            cv::Mat modifiedB = channelB.clone();

            modifiedA +=15;
            modifiedB += 30;
            std::vector<cv::Mat> mergedChannels = {channelL, modifiedA, modifiedB};
            cv::Mat coloredLab;
            cv::merge(mergedChannels, coloredLab);

            cv::cvtColor(coloredLab, result, cv::COLOR_Lab2BGR);


        }
        if (isRecording && video.isOpened()) {

            cv::Mat recordingFrame;

            if (result.channels() == 1) {
                cv::cvtColor(
                    result,
                    recordingFrame,
                    cv::COLOR_GRAY2BGR
                );
            }
            else {
                recordingFrame = result;
            }

            video.write(recordingFrame);
        }
        cv::imshow("Бордюр для Рябцева", result);

        int key = cv::waitKey(30);
        if (key ==27){
            break;
        } else if (key == ' '){
            displayMode = (displayMode + 1) % 3;
        } else if (key =='r' || key =='R' || key == 1082 || key ==1042){
            applyRotation = !applyRotation;
        }else if (key == 'v' || key == 'V'){
            if (!isRecording){
                int codec = cv::VideoWriter::fourcc('M','J', 'P', 'G');
                video.open("Video.avi", codec, 30.0, result.size(), true);
            if (video.isOpened()){
                isRecording =true;
                std::cout <<"Начало записи!\n";
            }
            else{
                std::cerr <<"Ошбика записи\n";
            }
        }else{
            video.release();
            isRecording = false;

            std::cout <<"Конец записи!\n";
        }
    }
    }

    if (video.isOpened()) {
        video.release();
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}

