#include "common.h"

class LinePointFinder

{

private:
	//检测到的线段端点
	Point pointBegin;
	Point pointEnd;

	//检测参数
	int colorMode; //检测颜色模式
	int lineWidth; //检测的最小线宽
	int pointNum; //待拟合直线的最少数量
	int thresholdValue; //超彩色阈值分割阈值

public:

	//检测到的结果
	float distanceResult;
	float angleResult;

	LinePointFinder();
	void setDectParams(int colorInput, int widthInput, int numInput, int segThreshold);
	void segmentProcess(Mat &src, Mat &dst);
	void segmentProcess2(Mat &src, Mat &dst);
	int calculatePoint(Mat &src, Point &begin, Point &end);
	void drawDetectedLines(cv::Mat& image, cv::Scalar color);
	void calculateResult(Mat &src, Point pointA, Point pointB);
};



class CircleFinder
{
private:
	int colorMode;
	int minDist; //要检测圆形的最小圆心间隔
	int minRadius; //检测圆的最小半径
	int maxRadius; //检测圆的最大半径
	int circleLike; //泛化圆程度
	int thresholdValue; //超彩色颜色分割阈值

public:
	vector<Vec3f> circles; //霍夫检测得到的圆集合
	Point circleCenter; //检测到圆的圆心
	int radius; //检测到圆的半径

	CircleFinder();
	void setParams(int colorInput, int minDistValue, int minRadiusValue, int maxRadiusValue, int circleLikeValue, int segThreshold);
	void segmentProcess(Mat &src, Mat &dst);
	void segmentProcess2(Mat &src, Mat &dst);
	int circleCalculate(Mat &src);
	int circleCalculate2(Mat &src);
	
	int circleCalculate3(Mat &src);
	double circleDistance(Mat &src);
};


class ConfigFileRead
{
private:

	ifstream configFile;

public:

	ConfigFileRead();
	int readConfigFile(string filePath);
	string readValue(string keyWords);
	int closeConfigFile();
};
