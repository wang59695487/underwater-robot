#ifndef __COMMON__
#define __COMMON__
#pragma comment(lib,"pthreadVC2.lib") // linking DLL dependencies

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <semaphore.h> // semaphore, for multithreads

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <cmath>
#include <string>
#include "crc16.hpp"
#include "unistd.h"
#include <fstream> // c++文件操作
#include <iomanip> // 设置输出格式




#define cvQueryHistValue_1D( hist, idx0 )  ((float)cvGetReal1D( (hist)->bins, (idx0)))
using namespace std;
using namespace cv;
using namespace cv::dnn;
using namespace cv::ml;
#endif

#define RED 1
#define GREEN 2
#define BLUE 3
#define START_CIRCLE_DETECT 200
#define IMAGE_ROWS 30
#define IMAGE_COLS 30


#define PI 3.1415926
