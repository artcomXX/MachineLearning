#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <opencv2/imgproc.hpp>

int main(int argc, char* argv[])
{
    cv::Mat logo = cv::imread("../logo.png", cv::IMREAD_UNCHANGED);

    cv::Mat frame;
    cv::VideoCapture cap;

    int deviceID = 0;
    int apiID = cv::CAP_ANY;

    cap.open(deviceID, apiID);

    if (!cap.isOpened()){
        std::cerr<< "Ошибка, открой камеру!\n";
        return -1;
    }

    std::cout << "Начало"<<  std::endl
        << "Нажмите на кнопку ESC чтобы выйти" << std::endl
        << "Нажмине на 'c' для вывода логотипа" << std::endl;
    

    cv::namedWindow("Гастон ты жив!", cv::WINDOW_NORMAL);
    cv::resizeWindow("Гастон ты жив!", 1280, 720);
    bool showImage = false;

    for (;;)
    {
        cap.read(frame);
        if (frame.empty()){
            std::cerr << "Ошибка. Захвачен черный кадр\n";
            break;
        }
        
        if (showImage && !logo.empty()) {
            if (logo.rows <= frame.rows && logo.cols <= frame.cols) {
                cv::Mat roi = frame(cv::Rect(0, 0, logo.cols, logo.rows));
                
                if (logo.channels() == 4) {
                    std::vector<cv::Mat> ch;
                    cv::split(logo, ch);
                    cv::Mat bgr, mask = ch[3];
                    cv::merge(std::vector<cv::Mat>{ch[0], ch[1], ch[2]}, bgr);
                    bgr.copyTo(roi, mask);
                } else {
                    logo.copyTo(roi);
                }
            }
        }

        cv::imshow("Гастон ты жив!", frame);

        int k = cv::waitKey(5);
        k = k & 0xFF;
        if (k == 27) { 
            break;
        }
        else if (k == 'c' || k == 'C'){
            showImage = !showImage;
            std::cout << "showImage = " << showImage << "\n";  // отладка
}
        
    }
    return 0;
}