#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <set>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>

#include <libutils/rasserts.h>
#include <libutils/fast_random.h>
#include <string>

// Эта функция говорит нам правда ли пиксель отмаскирован, т.е. отмечен как "удаленный", т.е. белый
bool isPixelMasked(cv::Mat mask, int i, int j) {
    rassert(j >= 0 && j < mask.rows, 372489347280017);
    rassert(i >= 0 && i < mask.cols, 372489347280018);
    rassert(mask.type() == CV_8UC3, 2348732984792380019);

    // TODO проверьте белый ли пиксель
    cv::Vec3b color = mask.at<cv::Vec3b>(j, i);
    if(color[0] + color[1] + color[2] == 255 * 3)
        return true;
    return false;
}

double estimateQuality(cv::Mat image, int i, int j, int nx, int ny, int i_size, int j_size){
    double diff = 0;
    int j0 = j - j_size / 2;
    int i0 = i - i_size / 2;
    int ny0 = ny - j_size / 2;
    int nx0 = nx - i_size / 2;


    for(int j_it = 0; j_it <= j_size; j_it++){
        for(int i_it = 0; i_it <= i_size; i_it++){
            cv::Vec3b color_init = image.at<cv::Vec3b>(j0 + j_it, i0 + i_it);
            cv::Vec3b color_this = image.at<cv::Vec3b>(ny0 + j_it, nx0 + i_it);
            for(int k = 0; k < 3; k++)
                diff += pow(color_this[k] - color_init[k], 2);
        }
    }
    return diff;
}

void run(int caseNumber, std::string caseName) {
    std::cout << "_________Case #" << caseNumber << ": " <<  caseName << "_________" << std::endl;

    cv::Mat original = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_original.jpg");
    cv::Mat mask = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_mask.png");
    rassert(!original.empty(), 324789374290018);
    rassert(!mask.empty(), 378957298420019);

    // TODO напишите rassert сверяющий разрешение картинки и маски
    rassert(mask.rows == original.rows, "wrong comparing pictures sizes 324141462363")
    rassert(mask.cols == original.cols, "wrong comparing pictures sizes 41098420984092")
    // TODO выведите в консоль это разрешение картинки
     std::cout << "Image resolution: " << mask.rows;
     std::cout << " x ";
     std::cout << mask.cols << "\n";

    // создаем папку в которую будем сохранять результаты - lesson18/resultsData/ИМЯ_НАБОРА/
    std::string resultsDir = "lesson18/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    resultsDir += std::to_string(caseNumber) + "_" + caseName + "/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    // сохраняем в папку с результатами оригинальную картинку и маску
    cv::imwrite(resultsDir + "0original.png", original);
    cv::imwrite(resultsDir + "1mask.png", mask);

    // TODO замените белым цветом все пиксели в оригинальной картинке которые покрыты маской
    cv::Mat original_copy = original.clone();
    int cnt_masked_pix = 0;
    for(int i = 0; i < original_copy.rows; i++){
        for(int j = 0; j < original_copy.cols; j++){
            if(isPixelMasked(mask, i, j)) {
                original_copy.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
                cnt_masked_pix++;
            }
        }
    }
    // TODO сохраните в папку с результатами то что получилось под названием "2_original_cleaned.png"
    cv::imwrite(resultsDir + "2_original_cleaned.png", original_copy);
    // TODO посчитайте и выведите число отмаскированных пикселей (числом и в процентах) - в таком формате:
    // Number of masked pixels: 7899/544850 = 1%
    std::cout << "Number of masked pixels: " << cnt_masked_pix << "/" << original.cols*original.rows << " = "
    << cnt_masked_pix * 100 / (original.cols * original.rows) << "%";

    FastRandom random(32542341); // этот объект поможет вам генерировать случайные гипотезы

    // TODO 10 создайте картинку хранящую относительные смещения - откуда брать донора для заплатки, см. подсказки про то как с нею работать на сайте
    // TODO 11 во всех отмаскированных пикселях: заполните эту картинку с относительными смещениями - случайными смещениями (но чтобы они и их окрестность 5х5 не выходила за пределы картинки)
    // TODO 12 во всех отмаскированных пикселях: замените цвет пиксела А на цвет пикселя Б на который указывает относительное смещение пикселя А
    // TODO 13 сохраните получившуюся картинку на диск
    // TODO 14 выполняйте эти шаги 11-13 много раз, например 1000 раз (оберните просто в цикл, сохраняйте картинку на диск только на каждой десятой или сотой итерации)
    // TODO 15 теперь давайте заменять значение относительного смещения на новой только если новая случайная гипотеза - лучше старой, добавьте оценку "насколько смещенный патч 5х5 похож на патч вокруг пикселя если их наложить"
    //
    // Ориентировочный псевдокод-подсказка получившегося алгоритма:
    // cv::Mat shifts(...); // матрица хранящая смещения, изначально заполнена парами нулей
    // cv::Mat image = original; // текущая картинка
    // for (100 раз) {
    //     for (пробегаем по всем пикселям j,i) {
    //         if (если этот пиксель не отмаскирован)
    //             continue; // пропускаем т.к. его менять не надо
    //         cv::Vec2i dxy = смотрим какое сейчас смещение для этого пикселя в матрице смещения
    //         int (nx, ny) = (i + dxy.x, j + dxy.y); // ЭТО НЕ КОРРЕКТНЫЙ КОД, но он иллюстрирует как рассчитать координаты пикселя-донора из которого мы хотим брать цвет
    //         currentQuality = estimateQuality(image, j, i, ny, nx, 5, 5); // эта функция (создайте ее) считает насколько похож квадрат 5х5 приложенный центром к (i, j)
    //                                                                                                                        на квадрат 5х5 приложенный центром к (nx, ny)
    //
    //         int (rx, ry) = random.... // создаем случайное смещение относительно нашего пикселя, воспользуйтесь функцией random.next(...);
    //                                      (окрестность вокруг пикселя на который укажет смещение - не должна выходить за пределы картинки и не должна быть отмаскирована)
    //         randomQuality = estimateQuality(image, j, i, j+ry, i+rx, 5, 5); // оцениваем насколько похоже будет если мы приложим эту случайную гипотезу которую только что выбрали
    //
    //         if (если новое качество случайной угадайки оказалось лучше старого) {
    //             то сохраняем (rx,ry) в картинку смещений
    //             и в текущем пикселе кладем цвет из пикселя на которого только что смотрели (цент окрестности по смещению)
    //             (т.е. мы не весь патч сюда кладем, а только его центральный пиксель)
    //         } else {
    //             а что делать если новая случайная гипотеза хуже чем то что у нас уже есть?
    //         }
    //     }
    //     не забываем сохранить на диск текущую картинку
    //     а как численно оценить насколько уже хорошую картинку мы смогли построить? выведите в консоль это число
    // }

    cv::Mat shifts(original_copy.cols, original_copy.rows, CV_32SC2, cv::Scalar(0, 0));
    cv::Mat or_copy = original.clone();

    for(int cnt_c = 0; cnt_c < 1000; cnt_c++){
        for(int i = 0; i < original_copy.rows; i++){
            for(int j = 0; j < original_copy.cols; j++){
                if(!isPixelMasked(mask, i, j))
                    continue;

                cv::Vec2i dxy = shifts.at<cv::Vec2i>(i ,j);
                int nx = i + dxy[0];
                int ny = j + dxy[1];
                double currentQuality = estimateQuality(shifts, i, j, nx, ny, shifts.rows, shifts.cols);

                int rx = random.next() % shifts.rows - i;
                int ry = random.next() % shifts.cols - j;
                double randomQuality = estimateQuality(shifts, i, j, i + rx, j + ry, shifts.rows, shifts.cols);

                if(randomQuality > currentQuality){
                    shifts.at< cv::Vec2i>(i, j) = cv::Vec2i(rx + i, ry + j);
                }
            }
        }
    }
}


int main() {
    try {
        run(1, "mic");
        // TODO протестируйте остальные случаи:
//        run(2, "flowers");
//        run(3, "baloons");
//        run(4, "brickwall");
//        run(5, "old_photo");
//        run(6, "your_data"); // TODO придумайте свой случай для тестирования (рекомендуется не очень большое разрешение, например 300х300)

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
