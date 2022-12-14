#include "LineCircleDetect.hpp"
 
// 4.5 version2, add delay function

//////////////////////////////////////////////////////LinePointFinder
LinePointFinder::LinePointFinder() :colorMode(0), lineWidth(20), pointNum(50) {}

void LinePointFinder::setDectParams(int colorInput, int widthInput, int numInput, int segThreshold)//设置要检测的颜色类型，线宽最小值，最少点数等参数

{

	colorMode = colorInput;

	lineWidth = widthInput;

	pointNum = numInput;

	thresholdValue = segThreshold;


}

void LinePointFinder::segmentProcess(Mat &src, Mat &dst) //根据彩色空间模型实现图像的分割
{
	int i = 0;
	int j = 0;
	int temp_r = 0;
	int temp_g = 0;
	int temp_b = 0;
	int temp = 0;

	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
		{
			temp_r = (int)src.at<Vec3b>(i, j)[2];
			temp_g = (int)src.at<Vec3b>(i, j)[1];
			temp_b = (int)src.at<Vec3b>(i, j)[0];

			if (RED == colorMode) //red color
			{
				temp = temp_r * 2 - temp_g - temp_b;
			}
			else if (GREEN == colorMode) //green color
			{
				temp = temp_g * 2 - temp_r - temp_b;
			}
			else if (BLUE == colorMode) //blue color
			{
				temp = temp_b * 2 - temp_r - temp_g;
			}
			else //red color
			{
				temp = temp_r * 2 - temp_g - temp_b;
			}

			if (temp <= thresholdValue)
			{
				temp = 0;
			}
			else if (temp > thresholdValue)
			{
				temp = 255;
			}
			dst.at<uchar>(i, j) = temp;

		}

}

void LinePointFinder::segmentProcess2(Mat &src, Mat &dst)  //segment base on HSV model
{
	Mat imgHSV;
	int hValueMin, hValueMax, sValueMin, sValueMax, vValueMin, vValueMax;


	//	hValueMin = 30;
		//hValueMax = 90;
		//sValueMin = 43;
		//sValueMax = 255;
		//vValueMin = 46;
		//vValueMax = 255;

	switch (colorMode)
	{
	case RED:
		hValueMin = 156;
		hValueMax = 180;
		sValueMin = 43;
		sValueMax = 255;
		vValueMin = 46;
		vValueMax = 255;
		break;
	case GREEN:
		hValueMin = 35;
		hValueMax = 77;
		sValueMin = 43;
		sValueMax = 255;
		vValueMin = 30;//46;
		vValueMax = 180;//255
		break;
	case BLUE:
		hValueMin = 91;
		hValueMax = 130;
		sValueMin = 43;
		sValueMax = 255;
		vValueMin = 46;
		vValueMax = 255;
		break;
	default:
		hValueMin = 35;
		hValueMax = 77;
		sValueMin = 43;
		sValueMax = 255;
		vValueMin = 30;//46;
		vValueMax = 255;
		break;
	}
		

	cvtColor(src, imgHSV, CV_BGR2HSV);
	if (RED == colorMode || 0 == colorMode || 4 == colorMode)
	{
		Mat dst1, dst2;
		inRange(imgHSV, Scalar(hValueMin, sValueMin, vValueMin), Scalar(hValueMax, sValueMax, vValueMax), dst1);
		inRange(imgHSV, Scalar(0, sValueMin, vValueMin), Scalar(25, sValueMax, vValueMax), dst2);
		dst = dst1 + dst2;
	}
	else
	{
		inRange(imgHSV, Scalar(hValueMin, sValueMin, vValueMin), Scalar(hValueMax, sValueMax, vValueMax), dst);
	}
}


int LinePointFinder::calculatePoint(Mat &src, Point &begin, Point &end) //对分割图像进行形态学处理，拟合得到直线，根据线段长度对直线进行截断得到线段两端点
{
	vector<Point> findPoints;
	vector<Point> pointsX;
	vector<Point> pointsY;

	int i = 0;
	int j = 0;
	int k = 0;
	int xLeft;
	int xRight;
	int yUp;
	int yDowm;
	Point pointX;
	Point pointY;

	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));

	morphologyEx(src, src, MORPH_OPEN, element);



	vector<vector<Point>> lineContours;
	vector<Vec4i> hierarchy;
	Mat mask;
	mask = Mat::zeros(src.size(), CV_8UC1);

	findContours(src, lineContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	float areaContour = 0;
	float maxArea = 0;
	int index = 0;
	for (int i = 0; i < lineContours.size(); i++)
	{
		areaContour = contourArea(lineContours[i]);
		if (areaContour > maxArea)
		{
			maxArea = areaContour;
			index = i;
		}
	}
	if (maxArea == 0)
	{
		//cout << "No line find!!!" << endl;
		return -1;
	}

	Rect rectContour = boundingRect(lineContours[index]);
	mask(rectContour).setTo(255);
	Mat imageMask;
	src.copyTo(imageMask, mask);
	src = imageMask.clone();




	//沿X方向扫描
	for (i = 0; i < src.rows; i++)
	{
		xLeft = 0;
		xRight = src.cols - 1;
		for (j = 0; j < src.cols - lineWidth && j >= 0; j++)
		{
			if (src.at<uchar>(i, j) == 255)
			{
				xLeft = j;
				break;
			}
		}
		for (j = src.cols - 1; j >= lineWidth && j < src.cols; j--)
		{
			if (src.at<uchar>(i, j) == 255)
			{
				xRight = j;
				break;
			}
		}
		if (xRight >= xLeft + lineWidth && xRight <= xLeft + 7 * lineWidth)
		{
			pointX.x = (xLeft + xRight) / 2;
			pointX.y = i;
			pointsX.push_back(pointX);
		}

	}

	//沿Y方向扫描
	for (i = 0; i < src.cols; i++)
	{
		yUp = 0;
		yDowm = src.rows - 1;
		for (j = 0; j < src.rows - lineWidth && j >= 0; j++)
		{
			if (src.at<uchar>(j, i) == 255)
			{
				yUp = j;
				break;
			}
		}
		for (j = src.rows - 1; j >= lineWidth && j < src.rows; j--)
		{
			if (src.at<uchar>(j, i) == 255)
			{
				yDowm = j;
				break;
			}
		}
		if (yDowm >= yUp + lineWidth && yDowm <= yUp + 7 * lineWidth)
		{
			pointY.x = i;
			pointY.y = (yUp + yDowm) / 2;
			pointsY.push_back(pointY);
		}

	}

	if (pointsX.size() < pointNum && pointsY.size() < pointNum)
	{
		//cout << "No line find!!!" << endl;
		return -1;
	}
	Vec4f line_detect;

	if (pointsX.size() >= pointsY.size())
		findPoints = pointsX;
	else
		findPoints = pointsY;

	int  len = findPoints.size();

	fitLine(findPoints, line_detect, CV_DIST_L2, 0, 0.01, 0.01);

	for (i = 0; i < len - 1; i++)
		line(src, findPoints[i], findPoints[i + 1], Scalar(128, 0, 0));

	int x0 = line_detect[2];
	int y0 = line_detect[3];

	int pointDisX = findPoints[len - 1].x - findPoints[0].x;
	int pointDisY = findPoints[len - 1].y - findPoints[0].y;

	int pointDis = pow((pointDisX*pointDisX + pointDisY * pointDisY), 0.5);

	pointBegin.x = x0 + pointDis / 2 * line_detect[0];
	pointBegin.y = y0 + pointDis / 2 * line_detect[1];

	pointEnd.x = x0 - pointDis / 2 * line_detect[0];
	pointEnd.y = y0 - pointDis / 2 * line_detect[1];

	if (pointBegin.y > pointEnd.y)
	{
		begin.x = pointBegin.x;
		begin.y = pointBegin.y;

		end.x = pointEnd.x;
		end.y = pointEnd.y;
	}
	else if (pointBegin.y < pointEnd.y)
	{
		begin.x = pointEnd.x;
		begin.y = pointEnd.y;

		end.x = pointBegin.x;
		end.y = pointBegin.y;

	}
	else
	{
		if (pointBegin.x <= pointEnd.x)
		{
			begin.x = pointBegin.x;
			begin.y = pointBegin.y;

			end.x = pointEnd.x;
			end.y = pointEnd.y;
		}
		else
		{
			begin.x = pointEnd.x;
			begin.y = pointEnd.y;

			end.x = pointBegin.x;
			end.y = pointBegin.y;
		}
	}

	return 0;
}


void LinePointFinder::drawDetectedLines(cv::Mat& image, cv::Scalar color = cv::Scalar(255, 0, 0)) //在图像中划出检测到的直线

{
	cv::line(image, pointBegin, pointEnd, color);

}

void LinePointFinder::calculateResult(Mat &src, Point pointA, Point pointB) //计算图像中心到检测到的直线的距离和直线方向角，直线方向为 pointA->pointB
{
	int width = src.cols;
	int height = src.rows;

	Point center;
	center.x = cvRound(width / 2);
	center.y = cvRound(height / 2);

	int A = 0, B = 0, C = 0;
	A = pointA.y - pointB.y;
	B = pointB.x - pointA.x;
	C = pointA.x*pointB.y - pointA.y*pointB.x;

	distanceResult = -((float)(A*center.x + B * center.y + C)) / ((float)sqrtf(A*A + B * B));

	float temp = (float)(pointA.y - pointB.y) / (float)sqrtf((pointB.x - pointA.x)*(pointB.x - pointA.x) + (pointB.y - pointA.y)*(pointB.y - pointA.y));

	if (pointB.x >= pointA.x)
		angleResult = acosf(temp) * 180 / PI;
	else
		angleResult = -acosf(temp) * 180 / PI;

}
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////CircleFinder
CircleFinder::CircleFinder() {}

void CircleFinder::setParams(int colorInput, int minDistValue, int minRadiusValue, int maxRadiusValue, int circleLikeValue, int segThreshold) //脡猫脰脙脳卯脨隆脭虏脨脛鸥脿拢卢脳卯脨隆脭虏掳毛鸥露拢卢脳卯沤贸脭虏掳毛鸥露拢卢路潞禄炉露脠碌脠虏脦脢媒
{
	colorMode = colorInput;
	minDist = minDistValue;
	minRadius = minRadiusValue;
	maxRadius = maxRadiusValue;
	circleLike = circleLikeValue;
	thresholdValue = segThreshold;
}

void CircleFinder::segmentProcess(Mat &src, Mat &dst) //根据彩色空间模型实现图像的分割
{
	int i = 0;
	int j = 0;
	int temp_r = 0;
	int temp_g = 0;
	int temp_b = 0;
	int temp = 0;

	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
		{

			temp_r = (int)src.at<Vec3b>(i, j)[2];
			temp_g = (int)src.at<Vec3b>(i, j)[1];
			temp_b = (int)src.at<Vec3b>(i, j)[0];

			if (RED == colorMode) //red color
			{
				temp = temp_r * 2 - temp_g - temp_b;
			}
			else if (GREEN == colorMode) //green color
			{
				temp = temp_g * 2 - temp_r - temp_b;
			}
			else if (BLUE == colorMode) //blue color
			{
				temp = temp_b * 2 - temp_r - temp_g;
			}

			if (temp <= thresholdValue)
			{
				temp = 0;
			}
			else if (temp > thresholdValue)
			{
				temp = 255;
			}

			dst.at<uchar>(i, j) = temp;



		}

}

void CircleFinder::segmentProcess2(Mat &src, Mat &dst) //segment base on HSV model
{
	Mat imgHSV;
	int hValueMin, hValueMax, sValueMin, sValueMax, vValueMin, vValueMax;

		hValueMin = 18;
		hValueMax = 40;
		sValueMin = 116;
		sValueMax = 255;
		vValueMin = 60;
		vValueMax = 200;
		
	cvtColor(src, imgHSV, CV_BGR2HSV);
	if (RED == colorMode)
	{
		Mat dst1, dst2;
		inRange(imgHSV, Scalar(hValueMin, sValueMin, vValueMin), Scalar(hValueMax, sValueMax, vValueMax), dst1);
		inRange(imgHSV, Scalar(0, sValueMin, vValueMin), Scalar(15, sValueMax, vValueMax), dst2);
		dst = dst1 + dst2;
	}
	else
	{
		inRange(imgHSV, Scalar(hValueMin, sValueMin, vValueMin), Scalar(hValueMax, sValueMax, vValueMax), dst);
	}
}



bool ellipsefinder(Mat src, vector<Point> contour, RotatedRect ellipse, double ratio = 0.01)
{
	//得到椭圆上所有的点
	vector<Point> ellipse_point;

	//得到椭圆的参数
	Point2f center = ellipse.center;
	double a_2 = pow(ellipse.size.width * 0.5, 2);
	double b_2 = pow(ellipse.size.height * 0.5, 2);
	double ellipse_angle = (ellipse.angle * 3.1415926) / 180;


	//上部
	for (int i = 0; i < ellipse.size.width; i++)
	{
		double x = -ellipse.size.width * 0.5 + i;
		double y_left = sqrt((1 - (x * x / a_2)) * b_2);

		//rotate
		//[ cos(seta) sin(seta)]
		//[-sin(seta) cos(seta)]
		cv::Point2f rotate_point_left;
		rotate_point_left.x = cos(ellipse_angle) * x - sin(ellipse_angle) * y_left;
		rotate_point_left.y = +sin(ellipse_angle) * x + cos(ellipse_angle) * y_left;

		//trans
		rotate_point_left += center;

		//store
		ellipse_point.push_back(Point(rotate_point_left));
	}
	//下部
	for (int i = 0; i < ellipse.size.width; i++)
	{
		double x = ellipse.size.width * 0.5 - i;
		double y_right = -sqrt((1 - (x * x / a_2)) * b_2);

		//rotate
		//[ cos(seta) sin(seta)]
		//[-sin(seta) cos(seta)]
		cv::Point2f rotate_point_right;
		rotate_point_right.x = cos(ellipse_angle) * x - sin(ellipse_angle) * y_right;
		rotate_point_right.y = +sin(ellipse_angle) * x + cos(ellipse_angle) * y_right;

		//trans
		rotate_point_right += center;

		//存储
		ellipse_point.push_back(Point(rotate_point_right));

	}
	vector<vector<Point> > contours1;
	contours1.push_back(ellipse_point);
	//drawContours(src,contours1,-1,Scalar(255,0,0),2);

	//match shape
	double a0 = matchShapes(ellipse_point, contour, CV_CONTOURS_MATCH_I1, 0);
	if (a0 > 0.01)
	{
		return true;
	}

	return false;
}




int CircleFinder::circleCalculate(Mat &src) //从分割图像中检测满足条件的最大的圆，赋值给类里的圆心和半径成员变量，并在图像中画出
{

	Mat imgBlur(src.rows, src.cols, CV_8UC1);

	radius = 0;
	int index = 0;

	GaussianBlur(src, imgBlur, Size(9, 9), 2, 2); //高斯滤波


//		imshow("circle segment", imgGray);
//		waitKey(1);

	HoughCircles(imgBlur, circles, HOUGH_GRADIENT, 1, minDist, 100, circleLike, minRadius, maxRadius);


	Mat src_gray = imgBlur;

	Mat threshold_output;
	vector<vector<Point> > contours;

	//寻找轮廓
	int threshold_value = threshold(src_gray, threshold_output, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	findContours(threshold_output, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


	//拟合椭圆
	vector<RotatedRect> minEllipse(contours.size());
	int count = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		//point size check
		if (contours[i].size() < 10)
		{
			continue;
		}

		//point area
		if (contourArea(contours[i]) < 600)
		{
			continue;
		}

		minEllipse[i] = fitEllipse(Mat(contours[i]));


		//check shape
		if (ellipsefinder(src, contours[i], minEllipse[i]))
		{
			continue;
		}

		Scalar color = Scalar(0, 0, 255);
		// ellipse

		//ellipse(src, minEllipse[i], color, 3);
		count++;
	}

	if (0 == circles.size() && count == 0)
	{
		//cout << "No match circle find!!!" << endl;
		return -1;
	}

	else if (0 != circles.size() && count == 0)
	{
		for (int i = 0; i < circles.size(); i++)
		{
			if (circles[i][2] > radius)
			{
				radius = circles[i][2];
				index = i;
			}
		}

		circleCenter.x = cvRound(circles[index][0]);
		circleCenter.y = cvRound(circles[index][1]);

		radius = cvRound(circles[index][2]);

		//circle(src, circleCenter, radius, Scalar(255, 255, 0), 3, 8, 0);
	}
	circles.clear();
	return 0;
}


int CircleFinder::circleCalculate2(Mat &src)
{
	//return -1;
	cout << "circle start" << endl;

	Mat imgBlur;
	GaussianBlur(src, imgBlur, Size(5, 5), 2, 2);

	//Canny(imgBlur, edge_image, 30, 70);
	//edge_image = imgBlur;
	radius = 0;

	//imshow("bin",bin);
	//imshow("edge_image", edge_image);


	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgBlur, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);



	/*
	 * only examine the largest*/
	cout << "contours num: " << contours.size() << endl;
	if (contours.size() < 1 || contours.size() > 40) {

		//cout<<endl<<"too many"<<endl;
		return -1;
	}

	float curr_area, max_area = 0;
	int max_idx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		curr_area = contourArea(contours[i]);
		if (curr_area > max_area) {
			max_area = curr_area;
			max_idx = i;
		}
	}

	cout << "contour: " << contours.size();

	float areaContour = 0;
	Point2f centerDetect;
	float radiusDetect;
	Point2f centerDetectMax;
	float radiusDetectMax = 0;

	//for (int i = 0; i < contours.size(); i++) 
	//{
	areaContour = contourArea(contours[max_idx]);

	cout << endl;


	//cout<<areaContour<<endl;
	minEnclosingCircle(contours[max_idx], centerDetect, radiusDetect);
	//cout<<"count: "<<contours.size();
	//cout << "radiusDetect: " << radiusDetect << endl;
	//cout << "areaContour: " << areaContour << endl;
	//cout << "radiusMin: " << minRadius << endl;
	//cout << "radiusMax: " << maxRadius << endl;


	if (areaContour <= imgBlur.rows*imgBlur.cols/100)
	{
		cout<<"contour too samll"<<endl;
		return -1;
	}
	if(PI*radiusDetect*radiusDetect <= imgBlur.rows*imgBlur.cols/80)
	{
		cout<<"circle too samll"<<endl;
		return -1;
	}
	if(PI*radiusDetect*radiusDetect >= imgBlur.rows*imgBlur.cols/9)
	{
		cout<<"circle too samll"<<endl;
		return -1;
	}
	if(	PI*radiusDetect*radiusDetect > areaContour*2.5)
	{
		cout<<"cont not circle like"<<endl;
		return -1;
	}
	{
		if (PI*radiusDetect*radiusDetect < imgBlur.cols*imgBlur.rows / 3)
		{
			radiusDetectMax = radiusDetect;
			centerDetectMax.x = centerDetect.x;
			centerDetectMax.y = centerDetect.y;

			cout << endl << endl << endl << "detected" << endl << endl;
		}
	}

	//}
	if (radiusDetectMax > 0)
	{
		radius = cvRound(radiusDetectMax);
		circleCenter.x = cvRound(centerDetectMax.x);
		circleCenter.y = cvRound(centerDetectMax.y);
		circle(src, circleCenter, radius, Scalar(255, 255, 0), 3, 8, 0);
		//imshow("circle",src);
		static int cnt = 0;
		String imageBaseName = "result/MarkResult_";
		String imageName = imageBaseName + to_string(cnt) + ".jpg";
		cnt++;
		imwrite(imageName, src);
		return 0;
	}
	else
	{
		cout << "No match circle find!!!" << endl;
		return -1;
	}
	//circle(src, Point(circleCenter), radius, Scalar(255), 2);


}



double CircleFinder::circleDistance(Mat &src) //检测圆的圆心到图像中心的距离 + 半径
{
	double distance = 0;
	double distance_x = circleCenter.x - src.cols;
	double distance_y = circleCenter.y - src.rows;
	// distance=sqrt(distance_x*distance_x+distance_y*distance_y);

	return distance;
}
/////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////ConfigFileRead
ConfigFileRead::ConfigFileRead() {}
int ConfigFileRead::readConfigFile(string filePath)//读取本地参数配置文件
{
	configFile.open(filePath.c_str());
	if (configFile.is_open())
	{
		cout << "read config file successed!!!" << endl;
		return 0;
	}
	else
	{
		cout << "read config file failed!!!" << endl;
		return -1;
	}

}

string ConfigFileRead::readValue(string keyWords)//读取本地参数配置文件中相应KeyWords的值
{
	if (!configFile.is_open())
	{
		return{};
	}
	streampos pos = configFile.tellg();
	string strLine;
	string value;
	while (!configFile.eof())
	{
		getline(configFile, strLine);

		if ('#' == strLine[0])
		{
			continue;
		}

		size_t pose = strLine.find('=');
		string key = strLine.substr(0, pose);

		if (key == keyWords)
		{
			value = strLine.substr(pose + 1);
			configFile.seekg(pos);
			return value;
		}

	}
	configFile.seekg(pos);
	return{};
}

int ConfigFileRead::closeConfigFile()//关闭本地配置文件
{
	configFile.close();
	if (configFile.is_open())
		return -1;
	return 0;
}
