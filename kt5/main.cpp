#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>

int main(int argc, char** argv) {
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
    cv::Mat rotatedFrame;
    
    bool isColorModified = false;
    bool applyRotation = false;

    int colorMode = 0;

    while(true){
        cap >> frame;

        if(frame.empty()){
            break;
        }

        if(applyRotation){        
            cv::rotate(frame, rotatedFrame, cv::ROTATE_90_CLOCKWISE);

        }else {
            rotatedFrame = frame;
        }

        if (colorMode != 0) {

            for (int y = 0; y < rotatedFrame.rows; y++) {

                for (int x = 0; x < rotatedFrame.cols; x++) {

                    cv::Vec3b& pixel =
                        rotatedFrame.at<cv::Vec3b>(y, x);

                    if (colorMode == 1) {
                        pixel[1] = cv::saturate_cast<uchar>(pixel[1] * 1.30);
                    }
                    else if (colorMode == 2) {
                        pixel[2] = cv::saturate_cast<uchar>(pixel[2] * 1.30);
                    }
                    else if (colorMode == 3) {
                        pixel[0] = cv::saturate_cast<uchar>(pixel[0] * 1.30);
                    }
                }
            }
        }


        cv::imshow("Зерноград - столица мира!", rotatedFrame);

        int key = cv::waitKey(30);
        if (key ==27){
            break;
        }else if (key =='r' || key =='R' || key == 1082 || key ==1042){
            applyRotation = !applyRotation;
        }else if (key == 's'|| key =='S'){
            colorMode++;
            if (colorMode > 3) {
                colorMode = 0;
            }
        } 
    }
    cap.release();
    cv::destroyAllWindows();

    return 0;
}

