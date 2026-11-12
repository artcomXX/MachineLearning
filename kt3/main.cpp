#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>
#include <string>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Не удалось открыть веб-камеру!" << std::endl;
        return -1;
    }

    std::string yunetPath = "../face_detection_yunet.onnx";
    cv::Ptr<cv::FaceDetectorYN> detector = cv::FaceDetectorYN::create(
        yunetPath, "", cv::Size(320, 320), 0.9f, 0.3f, 5
    );

    std::string emotionModelPath = "../emotion-ferplus-8.onnx";
    cv::dnn::Net emotionNet = cv::dnn::readNetFromONNX(emotionModelPath);
    if (emotionNet.empty()) {
        std::cerr << "Не удалось загрузить модель эмоций!" << std::endl;
        return -1;
    }

    const std::vector<std::string> emotions = {
        "Neutral", "Happiness", "Surprise", "Sadness",
        "Anger", "Disgust", "Fear", "Contempt"
    };

    cv::Mat frame;

    bool recording = false;

    int faceCount = 0;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        detector->setInputSize(frame.size());
        cv::Mat faces;
        detector->detect(frame, faces);

        for (int i = 0; i < faces.rows; i++) {
            int x = static_cast<int>(faces.at<float>(i, 0));
            int y = static_cast<int>(faces.at<float>(i, 1));
            int w = static_cast<int>(faces.at<float>(i, 2));
            int h = static_cast<int>(faces.at<float>(i, 3));

            cv::Rect faceRect = cv::Rect(x, y, w, h) & cv::Rect(0, 0, frame.cols, frame.rows);
            if (faceRect.empty()) continue;

            cv::Mat faceROI = frame(faceRect);

            if (recording) {
                std::string filename = "../video_faces/face_" +
                    std::to_string(faceCount++) + ".jpg";

                cv::imwrite(filename, faceROI);
            }
            cv::Mat grayFace;
            cv::cvtColor(faceROI, grayFace, cv::COLOR_BGR2GRAY);

            cv::Mat blob = cv::dnn::blobFromImage(
                grayFace, 1.0, cv::Size(64, 64), cv::Scalar(0), false, false
            );

            emotionNet.setInput(blob);
            cv::Mat prob = emotionNet.forward();

            cv::Point classIdPoint;
            double maxVal;
            cv::minMaxLoc(prob, nullptr, &maxVal, nullptr, &classIdPoint);
            std::string label = emotions[classIdPoint.x];

            cv::rectangle(frame, faceRect, cv::Scalar(0, 255, 0), 2);
            cv::putText(
                frame, label,
                cv::Point(faceRect.x, std::max(20, faceRect.y - 10)),
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2
            );
        }

        cv::imshow("Груз 'Рябцев'", frame);

        int key = cv::waitKey(1);

        if (key == 27) break;

        if (key == 'r' || key == 'R') {
            recording = !recording;

            if (recording) {
                std::cout << "Фото попёрли!" << std::endl;
            } else {
                std::cout << "Фото остановились!" << std::endl;
            }
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

