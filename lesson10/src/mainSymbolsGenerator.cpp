#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "hog.h"

#include <opencv2/imgproc.hpp>


#define NSAMPLES_PER_LETTER 5
#define LETTER_DIR_PATH std::string("lesson10/generatedData/letters")
const double INF = 1000000000;

int randFont() {
    int fonts[] = {
            cv::FONT_HERSHEY_SIMPLEX,
            cv::FONT_HERSHEY_PLAIN,
            cv::FONT_HERSHEY_DUPLEX,
            cv::FONT_HERSHEY_COMPLEX,
            cv::FONT_HERSHEY_TRIPLEX,
            cv::FONT_HERSHEY_COMPLEX_SMALL,
            cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
            cv::FONT_HERSHEY_SCRIPT_COMPLEX,
    };
    // Выбираем случайный шрифт из тех что есть в OpenCV
    int nfonts = (sizeof(fonts) / sizeof(int));
    int font = rand() % nfonts;

    // С вероятностью 20% делаем шрифт наклонным (italic)
    bool is_italic = ((rand() % 5) == 0);
    if  (is_italic) {
        font = font | cv::FONT_ITALIC;
    }

    return font;
}

double randFontScale() {
    double min_scale = 2.5;
    double max_scale = 3.0;
    double scale = min_scale + (max_scale - min_scale) * ((rand() % 100) / 100.0);
    return scale;
}

int randThickness() {
    int min_thickness = 2;
    int max_thickness = 3;
    int thickness = min_thickness + rand() % (max_thickness - min_thickness + 1);
    return thickness;
}

cv::Scalar randColor() {
    return cv::Scalar(rand() % 128, rand() % 128, rand() % 128); // можно было бы брать по модулю 255, но так цвета будут темнее и контрастнее
}

cv::Mat generateImage(std::string text, int width=128, int height=128) {
    cv::Scalar white(255, 255, 255);
    cv::Scalar backgroundColor = white;
    // Создаем картинку на которую мы нанесем символ (пока что это просто белый фон)
    cv::Mat img(height, width, CV_8UC3, backgroundColor);

    // Выберем случайные параметры отрисовки текста - шрифт, размер, толщину, цвет
    int font = randFont();
    double fontScale = randFontScale();
    int thickness = randThickness();
    cv::Scalar color = randColor();

    // Узнаем размер текста в пикселях (если его нарисовать с указанными параметрами)
    int baseline = 0;
    cv::Size textPixelSize = cv::getTextSize(text, font, fontScale, thickness, &baseline);

    // Рисуем этот текст идеально в середине картинки
    // (для этого и нужно было узнать размер текста в пикселях - чтобы сделать отступ от середины картинки)
    // (ведь при рисовании мы указываем координаты левого нижнего угла текста)
    int xLeft = (width / 2) - (textPixelSize.width / 2);
    int yBottom = (height / 2) + (textPixelSize.height / 2);
    cv::Point coordsOfLeftBorromCorner(xLeft, yBottom);
    cv::putText(img, text, coordsOfLeftBorromCorner, font, fontScale, color, thickness);

    return img;
}

void generateAllLetters() {
    srand(239017); // фиксируем зерно генератора случайных чисел (чтобы картинки от раза к разу генерировались с одинаковыми шрифтами, размерами и т.п.)

    for (char letter = 'a'; letter <= 'z'; ++letter) {

        // Создаем папку для текущей буквы:
        std::string letterDir = LETTER_DIR_PATH + "/" + letter;
        std::filesystem::create_directory(letterDir);

        for (int sample = 1; sample <= NSAMPLES_PER_LETTER; ++sample) {
            std::string text = std::string("") + letter;
            cv::Mat img = generateImage(text);

            cv::blur(img, img, cv::Size(3, 3));

            std::string letterSamplePath = letterDir + "/" + std::to_string(sample) + ".png";
            cv::imwrite(letterSamplePath, img);
        }
    }
}


void experiment1() {
    // TODO Проведите эксперимент 1:
    // Пробежав в цикле по каждой букве - посчитайте насколько сильно она отличается между своими пятью примерами? (NSAMPLES_PER_LETTER)
    // Для каждой буквы выведите:
    // 1) Среднее попарное расстояние (среднюю похожесть) между всеми примерами этой буквы
    // 2) Максимальное попарное расстояние между примерами этой буквы
    //
    // А так же среди всех максимальных расстояний найдите максимальное и выведите его в конце

    std::cout << "________Experiment 1________" << std::endl;
    for (char letter = 'a'; letter <= 'z'; ++letter) {
        std::string letterDir = LETTER_DIR_PATH + "/" + letter;

        double distMax = 0.0, distSum = 0.0, distN = NSAMPLES_PER_LETTER;
        for (int sampleA = 1; sampleA <= NSAMPLES_PER_LETTER; ++sampleA) {
            for (int sampleB = sampleA + 1; sampleB <= NSAMPLES_PER_LETTER; ++sampleB) {
                cv::Mat a = cv::imread(letterDir + "/" + std::to_string(sampleA) + ".png");
                cv::Mat b = cv::imread(letterDir + "/" + std::to_string(sampleB) + ".png");
                HoG hogA = buildHoG(a);
                HoG hogB = buildHoG(b);

                double dist = distance(hogA, hogB);
                distMax = std::max(dist, distMax);
                distSum += dist;
                // TODO
            }
        }
        std::cout << "Letter " << letter << ": max=" << distMax << ", avg=" << (distSum / distN) << std::endl;
    }
}


inline double average_dist(std::vector<HoG> arr_a, std::vector<HoG> arr_b){
    double sum_dist = 0.0;
    int cnt = 0;
    for(int i  = 0; i < NSAMPLES_PER_LETTER; i++){
        for(int j = 0; j < NSAMPLES_PER_LETTER; j++){
            double tmp_dist = distance(arr_a[i], arr_b[j]);
            sum_dist += tmp_dist;
            cnt++;
        }
    }
    return sum_dist / cnt;
}


void experiment2() {
    // TODO Проведите эксперимент 2:
    // Для каждой буквы найдите среди остальных наиболее похожую и наименее похожую
    //
    // А так же среди всех минимальных расстояний найдите среднее и выведите его в конце
    //  - Посмотрите и подумайте: как это число соотносится с максимальным расстоянием из прошлого эксперимента?
    //  - Какие буквы невозможно различить закодировав их в HoG?
    //  - Можно ли с этим что-то сделать?

    std::cout << "________Experiment 2________" << std::endl;
    for (char letterA = 'a'; letterA <= 'z'; ++letterA) {
        std::string letterDirA = LETTER_DIR_PATH + "/" + letterA + "/";


        char letterMax = 'a';
        double distMax = 0.0;
        char letterMin = 'b';
        double distMin = INF;

        //std::cout << letterDirA;
        std::vector<HoG> hogsA(NSAMPLES_PER_LETTER);
        for(int i = 0; i < NSAMPLES_PER_LETTER; i++){
            cv::Mat a = cv::imread(letterDirA + std::to_string(i) + ".png");
            HoG hogA = buildHoG(a);
            hogsA[i] = hogA;
        }


        for (char letterB = 'a'; letterB <= 'z'; ++letterB) {
            if (letterA == letterB) continue;


            std::string letterDirB = LETTER_DIR_PATH + "/" + letterB + "/";
            std::vector<HoG> hogsB(NSAMPLES_PER_LETTER);
            for(int i = 0; i < NSAMPLES_PER_LETTER; i++){
                cv::Mat b = cv::imread(letterDirB + std::to_string(i) + ".png");
                HoG hogB = buildHoG(b);
                hogsB[i] = hogB;
            }


            double dist = average_dist(hogsA, hogsB);


            if(dist > distMax){
                distMax = dist;
                letterMax = letterB;
            }

            if(dist < distMin){
                distMin = dist;
                letterMin = letterB;
            }
            // TODO
        }

        std::cout << "Letter " << letterA << ": max=" << letterMax << "/" << distMax << ", min=" << letterMin << "/" << distMin << std::endl;
    }
}


int main() {
    try {
        std::cout << "Generating letters images..." << std::endl;

        generateAllLetters();

        std::cout << "Images with letters were generated!" << std::endl;

        // TODO:
        //experiment1();

        // TODO:
        experiment2();


        std::cout << "\n" << "\n" << "\n";
        std::string letterDir1 = LETTER_DIR_PATH + "/" + "b" + "/1";
        cv::Mat a1 = cv::imread(letterDir1 + ".png");
        std::string letterDir2 = LETTER_DIR_PATH + "/" + "p" + "/1";
        cv::Mat a2 = cv::imread(letterDir2 + ".png");
        HoG hog1 = buildHoG(a1);
        HoG hog2 = buildHoG(a2);
        std::cout << hog1 << "\n";
        std::cout << hog2 << "\n";
        std::cout << distance(hog1, hog2);

    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

