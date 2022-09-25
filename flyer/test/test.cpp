#include <opencv2/opencv.hpp>

using namespace cv;

main()
{
VideoCapture cap(1);
Mat frame;
while(1)
{
cap>>frame;
imshow("frame",frame);
waitKey(10); 
}
}
