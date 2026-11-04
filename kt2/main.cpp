/*
 * ============================================================================
 *  Поиск прямоугольников на изображении и определение угла их поворота
 * ============================================================================
 *
 *  Программа находит прямоугольные объекты на изображении,
 *  определяет их координаты, центр и угол поворота относительно горизонта.
 *
 *  Идея (пайплайн обработки изображения):
 *
 *    BGR --> HSV --> Маска --> Контуры --> Прямоугольники
 *         --> Центры и углы поворота --> Результат
 *
 *  Сборка:
 *      g++ main.cpp -o detector `pkg-config --cflags --libs opencv4`
 *
 *  Запуск:
 *      ./detector /полный/путь/к/boxes.png
 *
 *  Управление:
 *      q / Esc - выход
 *      s       - сохранить результат
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include <opencv2/opencv.hpp>

// ------------------------------------------------------------------
// Функция отображения изображения.
//
// В Python использовались PIL и matplotlib.
// В C++ вместо них используем cv::imshow().
// ------------------------------------------------------------------

void showImage(const cv::Mat& img, const std::string& windowName)
{
    cv::imshow(windowName, img);
}

// ------------------------------------------------------------------
// Находим среди всех контуров только прямоугольники нужного размера.
//
// minArea - минимальная площадь прямоугольника.
// maxArea - максимальная площадь прямоугольника.
//
// minAreaRect() пытается вписать повёрнутый прямоугольник в контур.
// points() возвращает четыре вершины прямоугольника.
// contourArea() вычисляет площадь полученного прямоугольника.
// ------------------------------------------------------------------

std::vector<std::vector<cv::Point>> getBoxes(
    const std::vector<std::vector<cv::Point>>& contoursList,
    double minArea,
    double maxArea = 1000000
)
{
    std::vector<std::vector<cv::Point>> boxes;

    for (const auto& contour : contoursList)
    {
        cv::RotatedRect rect = cv::minAreaRect(contour);

        // Получаем четыре вершины прямоугольника.
        cv::Point2f points[4];
        rect.points(points);

        // Переводим координаты в целочисленный формат.
        std::vector<cv::Point> box;

        for (int i = 0; i < 4; i++)
        {
            box.push_back(cv::Point(
                cvRound(points[i].x),
                cvRound(points[i].y)
            ));
        }

        // Вычисляем площадь, занимаемую прямоугольником.
        double area = cv::contourArea(box);

        if (area > minArea && area < maxArea)
        {
            boxes.push_back(box);
        }
    }

    return boxes;
}

// ------------------------------------------------------------------
// Рисуем найденные прямоугольники на изображении.
// ------------------------------------------------------------------

cv::Mat drawContours(
    const cv::Mat& img,
    const std::vector<std::vector<cv::Point>>& boxes
)
{
    cv::Mat imgOut = img.clone();

    for (const auto& box : boxes)
    {
        cv::drawContours(
            imgOut,
            std::vector<std::vector<cv::Point>>{box},
            0,
            cv::Scalar(255, 0, 0),
            2
        );
    }

    return imgOut;
}

// ------------------------------------------------------------------
// Вычисляем центр прямоугольника и угол его поворота.
//
// Центр - среднее арифметическое координат четырёх вершин.
//
// edge1 и edge2 - две стороны прямоугольника.
//
// Выбираем наиболее длинную сторону и вычисляем угол
// между ней и горизонтальной осью.
// ------------------------------------------------------------------

std::pair<cv::Point, double> calcAngle(
    const std::vector<cv::Point>& box
)
{
    // Находим центр фигуры.
    cv::Point center(
        (box[0].x + box[1].x + box[2].x + box[3].x) / 4,
        (box[0].y + box[1].y + box[2].y + box[3].y) / 4
    );

    // Находим две стороны прямоугольника:
    // вертикальную и горизонтальную.
    cv::Point2f edge1(
        box[1].x - box[0].x,
        box[1].y - box[0].y
    );

    cv::Point2f edge2(
        box[2].x - box[1].x,
        box[2].y - box[1].y
    );

    // Определяем наиболее длинную сторону прямоугольника.
    cv::Point2f usedEdge;

    if (cv::norm(edge2) > cv::norm(edge1))
    {
        usedEdge = edge2;
    }
    else
    {
        usedEdge = edge1;
    }

    // Определяем горизонтальную сторону.
    cv::Point2f reference(1, 0);

    // Вычисляем угол между самой длинной стороной
    // прямоугольника и горизонтальной осью.
    double denominator = cv::norm(reference) * cv::norm(usedEdge);

    double angle = 0.0;

    if (denominator > 0)
    {
        double cosine =
            (reference.x * usedEdge.x + reference.y * usedEdge.y)
            / denominator;

        // Защита от ошибок округления при вычислении acos().
        cosine = std::clamp(cosine, -1.0, 1.0);

        angle = 180.0 / CV_PI * std::acos(cosine);
    }

    return {center, angle};
}

// ------------------------------------------------------------------
// Рисуем центр прямоугольника и выводим величину угла наклона.
// ------------------------------------------------------------------

cv::Mat drawAngle(
    const cv::Mat& img,
    cv::Point center,
    double angle
)
{
    cv::Mat imgOut = img.clone();

    // Рисуем круг в центре прямоугольника.
    cv::circle(
        imgOut,
        center,
        3,
        cv::Scalar(10, 200, 10),
        5
    );

    // Выводим величину угла наклона.
    cv::putText(
        imgOut,
        std::to_string(static_cast<int>(angle)),
        cv::Point(center.x + 10, center.y - 10),
        cv::FONT_HERSHEY_SIMPLEX,
        0.8,
        cv::Scalar(0, 0, 0),
        2
    );

    return imgOut;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv)
{
    // ------------------------------------------------------------------
    // 1. Загружаем изображение.
    // ------------------------------------------------------------------

    if (argc < 2)
    {
        std::cerr << "Укажите путь к изображению!" << std::endl;
        std::cerr << "Пример: ./detector boxes.png" << std::endl;
        return -1;
    }

    std::string imagePath = argv[1];

    cv::Mat image = cv::imread(imagePath);

    if (image.empty())
    {
        std::cerr << "Не удалось открыть изображение: "
                  << imagePath << std::endl;
        return -1;
    }

    std::cout << "Изображение успешно загружено!" << std::endl;

    showImage(image, "Original image");

    // ------------------------------------------------------------------
    // 2. Найдём контуры изображений.
    //
    //    Параметры для маски.
    //    Диапазон HSV повторяет значения из Python-программы.
    // ------------------------------------------------------------------

    cv::Scalar hsvMin(0, 5, 0);
    cv::Scalar hsvMax(250, 250, 255);

    cv::Mat hsv;

    // Меняем цветовую модель с BGR на HSV.
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    // Применяем цветовой фильтр.
    cv::Mat imageFilter;

    cv::inRange(
        hsv,
        hsvMin,
        hsvMax,
        imageFilter
    );

    showImage(imageFilter, "HSV mask");

    // ------------------------------------------------------------------
    // 3. Найдём контуры всех фигур на изображении.
    // ------------------------------------------------------------------

    std::vector<std::vector<cv::Point>> contoursList;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(
        imageFilter.clone(),
        contoursList,
        hierarchy,
        cv::RETR_TREE,
        cv::CHAIN_APPROX_SIMPLE
    );

    std::cout << "Количество найденных фигур: "
              << contoursList.size() << std::endl;

    // Выводим координаты одной из фигур.
    if (contoursList.size() > 5)
    {
        std::cout << "Координаты одной из фигур:\n";

        for (const auto& point : contoursList[5])
        {
            std::cout << point << " ";
        }

        std::cout << std::endl;
    }
    else
    {
        std::cout << "Контур с индексом 5 отсутствует."
                  << std::endl;
    }

    // ------------------------------------------------------------------
    // 4. Находим среди всех контуров только прямоугольники.
    // ------------------------------------------------------------------

    std::vector<std::vector<cv::Point>> boxes =
        getBoxes(contoursList, 700, 150200);

    std::cout << "Количество найденных прямоугольников: "
              << boxes.size() << std::endl;

    // Наносим прямоугольники на исходное изображение.
    cv::Mat imageOut = drawContours(image, boxes);

    showImage(imageOut, "Detected rectangles");

    // ------------------------------------------------------------------
    // 5. Вычисляем центры прямоугольников и углы поворота.
    // ------------------------------------------------------------------

    cv::Mat imageDone = image.clone();

    // Сначала наносим все прямоугольники.
    imageDone = drawContours(imageDone, boxes);

    for (const auto& box : boxes)
    {
        // Вычисляем центр и угол поворота.
        auto [center, angle] = calcAngle(box);

        // Наносим центр и угол на изображение.
        imageDone = drawAngle(imageDone, center, angle);

        std::cout << "Центр: " << center
                  << " | Угол: " << angle << " градусов"
                  << std::endl;
    }

    // ------------------------------------------------------------------
    // 6. Показываем итоговое изображение.
    // ------------------------------------------------------------------

    showImage(imageDone, "Final result");

    // ------------------------------------------------------------------
    // 7. Управление программой.
    //
    //    q / Esc - выход.
    //    s       - сохранить результат.
    // ------------------------------------------------------------------

    int key = cv::waitKey(0) & 0xFF;

    if (key == 's')
    {
        cv::imwrite("result.png", imageDone);
        cv::imwrite("mask.png", imageFilter);

        std::cout << "Сохранено: result.png и mask.png"
                  << std::endl;
    }

    cv::destroyAllWindows();

    return 0;
}
