#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

main()
{
VideoCapture cap(1);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FPS, 60);
	int value = 0;

Mat frame;

/*

int ipAddress1, ipAddress2, ipAddress3, ipAddress4;
char cmdBuffer[200];

					        ipAddress1 = 192;
							ipAddress2 = 168;
							ipAddress3 = 8;
							ipAddress4 = 108;
							sprintf(cmdBuffer,"sudo ifconfig wlan0 %d.%d.%d.%d",ipAddress1,ipAddress2,ipAddress3,ipAddress4);
							cout<<"cmd is :"<<cmdBuffer<<endl;
														system("sudo ifconfig wlan0 down");
							system(cmdBuffer);
							system("sudo ifconfig wlan0 up");
*/

while(1)
{
cap>>frame;
cout<<"frame size is :"<<frame.cols<<" "<<frame.rows<<endl;

	value = cap.get(CV_CAP_PROP_FPS);
        cout<<"vakue is :"<<value<<endl;

imshow("frame",frame);
waitKey(10); 
}
}
