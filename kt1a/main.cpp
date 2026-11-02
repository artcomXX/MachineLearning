#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <chrono>

std::string getTimeString()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << now->tm_hour << ":"
        << std::setfill('0') << std::setw(2) << now->tm_min << ":"
        << std::setfill('0') << std::setw(2) << now->tm_sec;
    return oss.str();
}
void drawDemoTimeWithShadow(cv::Mat& frame)
{
    std::string text = getTimeString();
    cv::Point pos(30, 50);

    int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
    double scale = 1.0;

    cv::putText(frame, text, pos + cv::Point(6, 6), fontFace, scale,
                cv::Scalar(10, 10, 10), 6, cv::LINE_AA);

    cv::putText(frame, text, pos, fontFace, scale, 
        cv::Scalar(0, 215, 255), 2, cv::LINE_AA);
}
int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened()){
        std::cerr <<"Не удалось открыть!" << std::endl;
        return -1;
    }
    auto start = std::chrono::steady_clock::now();
    cv::Mat frame;
    std::cout <<" Запущено. Esc для выхода" << std::endl;

    while(true){
        cap >> frame;
        if (frame.empty()) break;

    

    drawDemoTimeWithShadow(frame);
    cv::imshow("Медалька для Рябцева", frame);

    if (cv::waitKey(30) == 27) break;
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}