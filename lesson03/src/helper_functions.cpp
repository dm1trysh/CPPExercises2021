#include "helper_functions.h"

#include <libutils/rasserts.h>


cv::Mat makeAllBlackPixelsBlue(cv::Mat image) {
    // TODO реализуйте функцию которая каждый черный пиксель картинки сделает синим

    // ниже приведен пример как узнать цвет отдельного пикселя - состоящий из тройки чисел BGR (Blue Green Red)
    // чем больше значение одного из трех чисел - тем насыщеннее его оттенок
    // всего их диапазон значений - от 0 до 255 включительно
    // т.е. один байт, поэтому мы используем ниже тип unsigned char - целое однобайтовое неотрицательное число
    //cv::Vec3b color = image.at<cv::Vec3b>(13, 5); // взяли и узнали что за цвет в пикселе в 14-ом ряду (т.к. индексация с нуля) и 6-ой колонке
    //unsigned char blue = color[0]; // если это число равно 255 - в пикселе много синего, если равно 0 - в пикселе нет синего
   // unsigned char green = color[1];
   // unsigned char red = color[2];

    // как получить белый цвет? как получить черный цвет? как получить желтый цвет?
    // поэкспериментируйте! например можете всю картинку заполнить каким-то одним цветом

    // пример как заменить цвет по тем же координатам
    //red = 255;
    // запустите эту версию функции и посмотрите на получившуюся картинку - lesson03/resultsData/01_blue_unicorn.jpg
    // какой пиксель изменился? почему он не чисто красный?
    //image.at<cv::Vec3b>(13, 5) = cv::Vec3b(blue, green, red);

    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            cv::Vec3b color = image.at<cv::Vec3b>(i, j);
            unsigned char blue = color[0];
            unsigned char green = color[1];
            unsigned char red = color[2];

            if(blue == 0 && red == 0 && green == 0){
                blue = 255;
                image.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
            }
        }
    }



    return image;
}

cv::Mat invertImageColors(cv::Mat image) {
    // TODO реализуйте функцию которая каждый цвет картинки инвертирует:
    // т.е. пусть ночь станет днем, а сумрак рассеется
    // иначе говоря замените каждое значение яркости x на (255-x) (т.к находится в диапазоне от 0 до 255)

    using namespace std;
    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            cv::Vec3b color = image.at<cv::Vec3b>(i, j);
            unsigned blue = color[0];
            unsigned green = color[1];
            unsigned red = color[2];
            blue = 255 - blue;
            green = 255 - green;
            red = 255 - red;
            image.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
        }
    }
    return image;
}

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background) {
    rassert(object.cols == background.cols, 341241251251351);
    rassert(object.rows == background.rows, 341241251251351);
    // TODO реализуйте функцию которая все черные пиксели картинки-объекта заменяет на пиксели с картинки-фона
    // т.е. что-то вроде накладного фона получится
    using namespace std;
    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            cv::Vec3b color_obj = object.at<cv::Vec3b>(i, j);
            cv::Vec3b color_bg = background.at<cv::Vec3b>(i, j);
            unsigned blue = color_obj[0];
            unsigned green = color_obj[1];
            unsigned red = color_obj[2];
            if(blue == 0 && green == 0 && red == 0){
                blue = color_bg[0];
                green = color_bg[1];
                red = color_bg[2];
            }
            object.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
        }
    }


    // гарантируется что размеры картинок совпадают - проверьте это через rassert, вот например сверка ширины:
    rassert(object.cols == background.cols, 341241251251351);
    rassert(object.rows == background.rows, 341241251251351);

    return object;
}

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground) {
    // теперь вам гарантируется что largeBackground гораздо больше - добавьте проверок этого инварианта (rassert-ов)
    using namespace std;
    rassert(object.cols < largeBackground.cols, 341241251251351);
    rassert(object.rows < largeBackground.rows, 341241251251351);

    int obj_beg_height = (largeBackground.rows - object.rows) / 2;
    int obj_beg_width = (largeBackground.cols - object.cols) / 2;
    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            cv::Vec3b color_obj = object.at<cv::Vec3b>(i, j);
            cv::Vec3b color_bg = largeBackground.at<cv::Vec3b>(obj_beg_height + i, obj_beg_width + j);

            unsigned blue = color_obj[0];
            unsigned green = color_obj[1];
            unsigned red = color_obj[2];

            if(blue == 0 && green == 0 && red == 0){
                blue = color_bg[0];
                green = color_bg[1];
                red = color_bg[2];
            }
            largeBackground.at<cv::Vec3b>(obj_beg_height + i, obj_beg_width + j) = cv::Vec3b(blue, green, red);
        }
    }
    // TODO реализуйте функцию так, чтобы нарисовался объект ровно по центру на данном фоне, при этом черные пиксели объекта не должны быть нарисованы

    return largeBackground;
}


cv::Mat addUnicornAtRandomPlace(cv::Mat object, cv::Mat largeBackground){

    using namespace std;
    rassert(object.cols < largeBackground.cols, 341241251251351);
    rassert(object.rows < largeBackground.rows, 341241251251351);

    int obj_beg_height_possible = largeBackground.rows - object.rows;
    int obj_beg_width_possible = largeBackground.cols - object.cols;
    int obj_beg_height = static_cast<int>((std::rand()*1.0*obj_beg_height_possible) / RAND_MAX);
    int obj_beg_width = static_cast<int>((std::rand()*1.0*obj_beg_width_possible) / RAND_MAX);

    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            cv::Vec3b color_obj = object.at<cv::Vec3b>(i, j);
            cv::Vec3b color_bg = largeBackground.at<cv::Vec3b>(obj_beg_height + i, obj_beg_width + j);

            unsigned blue = color_obj[0];
            unsigned green = color_obj[1];
            unsigned red = color_obj[2];

            if(blue == 0 && green == 0 && red == 0){
                blue = color_bg[0];
                green = color_bg[1];
                red = color_bg[2];
            }
            largeBackground.at<cv::Vec3b>(obj_beg_height + i, obj_beg_width + j) = cv::Vec3b(blue, green, red);
        }
    }
    return largeBackground;
}

cv::Mat replaceInRandomColors(cv::Mat object){
    const int max_color_value = 255;
    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            cv::Vec3b color_obj = object.at<cv::Vec3b>(i, j);

            unsigned blue = color_obj[0];
            unsigned green = color_obj[1];
            unsigned red = color_obj[2];

            if(blue == 0 && green == 0 && red == 0){
                blue = static_cast<int>((std::rand()*1.0*max_color_value) / RAND_MAX);
                green = static_cast<int>((std::rand()*1.0*max_color_value) / RAND_MAX);
                red = static_cast<int>((std::rand()*1.0*max_color_value) / RAND_MAX);
            }

            object.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
        }
    }
    return object;
}

bool isFilledBy1(std::vector<std::vector<int>>& arr, int idx_i, int idx_j, int i_max, int j_max){
    const int depth_of_search = 15;


    if((idx_i - depth_of_search < 0) || (idx_i + depth_of_search >= i_max)
    || (idx_j - depth_of_search < 0) || (idx_j + depth_of_search >= j_max)){
        return true;
    }

    int i_beg = idx_i - depth_of_search;
    int j_beg = idx_j - depth_of_search;
    int i_end = idx_i + depth_of_search;
    int j_end = idx_j + depth_of_search;
    int cnt_of_right_pixels = 0;
    int cnt_of_all_pixels = (i_end - i_beg) * (j_end - j_beg);

    for(int i = i_beg; i < i_end; i++){
        for(int j = j_beg; j < j_end; j++){
            if(arr[i][j] == 1)
                cnt_of_right_pixels++;
        }
    }

    const double right_ammount = 0.85 ;
    if(cnt_of_right_pixels >= (right_ammount * cnt_of_all_pixels))
        return true;
    return false;
}


cv::Mat videoWithoutInterference(cv::Mat object, std::vector<std::vector<int>>& difference_in_colors){
    std::vector<std::vector<int>> difference_in_colors_new(object.rows, std::vector<int>(object.cols));
    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            difference_in_colors_new[i][j] = difference_in_colors[i][j];
        }
    }


    //adding more 1 to fix the spots
    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            if(isFilledBy1(difference_in_colors, i, j, object.rows, object.cols)){
                difference_in_colors_new[i][j] = 1;
            }
        }
    }

    //changing 1-spots to 0-spots back
    /*for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            if(!isFilledBy1(difference_in_colors, i, j, object.rows, object.cols)){
                difference_in_colors_new[i][j] = 0;
            }
        }
    }*/

    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            cv::Vec3b color_obj = object.at<cv::Vec3b>(i, j);
            if(difference_in_colors_new[i][j] == 1){
                unsigned blue = color_obj[0];
                unsigned green = color_obj[1];
                unsigned red = color_obj[2];

                blue = 0;
                green = 255;
                red = 0;

                object.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
            }

            /*if(i >= 0 && i < object.rows && j >= 0 && j < object.cols){
                cv::Vec3b color_up = object.at<cv::Vec3b>(i - 1, j);
                cv::Vec3b color_down = object.at<cv::Vec3b>(i + 1, j);
                cv::Vec3b color_right = object.at<cv::Vec3b>(i, j + 1);
                cv::Vec3b color_left = object.at<cv::Vec3b>(i, j - 1);

                int blue = color_obj[0];
                int green = color_obj[1];
                int red = color_obj[2];
                blue = static_cast<int>((color_up[0] + color_down[0] + color_left[0] + color_right[0] + color_obj[0]) / 5.0);
                green = static_cast<int>((color_up[1] + color_down[1] + color_left[1] + color_right[1] + color_obj[1]) / 5.0);
                red = static_cast<int>((color_up[2] + color_down[2] + color_left[2] + color_right[2] + color_obj[2]) / 5.0);

                object.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
            }*/
        }
    }
    return object;
}


cv::Mat videoWithBackground(cv::Mat object, cv::Mat init_pict){
    using namespace std;
    //rassert(object.cols > background.cols || object.rows > background.rows, "wrong size of the background");

    vector<vector<int>> diff_in_colors(object.rows, vector<int>(object.cols, 0));

    for(int i = 0; i < object.rows; i++){
        for(int j = 0; j < object.cols; j++){
            cv::Vec3b color_obj = object.at<cv::Vec3b>(i, j);
            unsigned blue = color_obj[0];
            unsigned green = color_obj[1];
            unsigned red = color_obj[2];

            cv::Vec3b color_obj_init = init_pict.at<cv::Vec3b>(i, j);
            unsigned blue_init = color_obj_init[0];
            unsigned green_init = color_obj_init[1];
            unsigned red_init = color_obj_init[2];

            const int diff_between_colors = 18;

            if((abs((int)blue - (int)blue_init) <= diff_between_colors) && (abs((int)green - (int)green_init) <= diff_between_colors)
            && (abs((int)red - (int)red_init) <= diff_between_colors)){
                green = 255;
                red = 0;
                blue = 0;
                diff_in_colors[i][j] = 1;
            }

            object.at<cv::Vec3b>(i, j) = cv::Vec3b(blue, green, red);
        }
    }
    cv::Mat ready_object = videoWithoutInterference(object.clone(), diff_in_colors);
    return ready_object;
}