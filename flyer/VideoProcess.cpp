/* ************************************************************************
 *       Filename:  VideoProcess.cpp
 *    Description:
 *        Version:  1.0
 *        Created:  13/03/19 04:30:21
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (),
 *        Company:
 * ************************************************************************/
#include "VideoProcess.hpp"
#include "LineCircleDetect.hpp"

extern sem_t sem_lo; // semaphore for lower camera frame fetch and read
extern sem_t sem_up;
extern pthread_mutex_t work_mutex; // mutex for preemptive resource access

/// output, externed by Serial.cpp
float resultDistance = 0;
float resultAngle = 0;
int resultOutFlag = 0;
int markFlag = 0;
char markResult[4] = { 0 };
int detectFlag = 0; // for serial
char detectResult[8] = { 0X00 };
char detectResult2[8] = {0X00 };

int hastakenphoto = 0;
int ic2letters = 0;
float imuAngle = 0;

int detect_index[4]={0};

string addr_fl = "./Pic/Line/";
char str[5];
String letter = "letterpic";
String imageBaseName = "./Result/MarkResult_";
String imageName;
String imageBaseName1 = "./Pic/Circle/Black_Result_";
String imageName1;
String xyr = "./Pic/Circle/yxjball.txt";
String xyl = "./Letter/yxjletter.txt";//zimu
string sample = "./Letter/";

//config parameters
int segThresholdValue;
int lineWidthValue;
int pointNumValue;
int minDistValue;
int minRadiusValue;
int maxRadiusValue;
int circleLikeValue;
int markAreaSize;
int ret;

//global control variables
int lineFindState;
bool letter_detect_flag, obs_detect_flag;

int markCheck(Mat &src)
{
	int sum = 0;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (src.at<uchar>(i, j) == 255)
				sum++;
		}
	}
	if (sum >= 4)
		return 0;
	else
		return -1;
}


VideoProcess::VideoProcess()
{
	model = StatModel::load<ANN_MLP>("/home/pi/soft/flyer/Letter_300_train.xml");
}

void VideoProcess::fetch_frame(VideoProcess *videoProcess) {
	///mat declaration for imshow()
	cout << "Fetching Frame......" << endl;
	Mat img_lo;
	Mat img_up;

	/// fetch frame
	while (1) {

		videoProcess->cap_lo >> img_lo;
		sem_wait(&sem_lo);
		pthread_mutex_lock(&work_mutex);
		videoProcess->frame_lo = img_lo;
		sem_post(&sem_lo);
		pthread_mutex_unlock(&work_mutex);

		//cout<<"one frame from below ----"<<endl;
		//cout<<"frame size is :"<<videoProcess->frame_lo.rows<<" "<<videoProcess->frame_lo.cols<<endl;
		if(img_lo.empty()){
			cout<<"lower frame empty"<<endl;
			continue;
		}
		//imshow("frame_lo", img_lo);
		//waitKey(15);

		videoProcess->cap_up >> img_up;
		sem_wait(&sem_up);
		pthread_mutex_lock(&work_mutex);
		videoProcess->frame_up = img_up;
		sem_post(&sem_up);
		pthread_mutex_unlock(&work_mutex);

		//cout<<"one frame from below ----"<<endl;
		//cout<<"frame size is :"<<videoProcess->frame_lo.rows<<" "<<videoProcess->frame_lo.cols<<endl;
		if(img_up.empty()){
			cout<<"upper frame empty"<<endl;
			continue;
		}
		//imshow("frame_up", img_up);
		//waitKey(15);
		//usleep(100);
	}

}


void VideoProcess::run() {

	///read config file
	lineFindState = 0;
	obs_detect_flag = false;
	letter_detect_flag = false;
	ConfigFileRead configFileRead;
	ret = configFileRead.readConfigFile("/home/pi/soft/flyer/config.cfg");
	if (ret < 0)
	{
		cout << "config file read failed, program shutdown!" << endl;
		return;
	}
	segThresholdValue = atoi(configFileRead.readValue("segThresholdValue").c_str());
	lineWidthValue = atoi(configFileRead.readValue("lineWidth").c_str());
	pointNumValue = atoi(configFileRead.readValue("pointNum").c_str());
	minDistValue = atoi(configFileRead.readValue("minDistValue").c_str());
	minRadiusValue = atoi(configFileRead.readValue("minRadiusValue").c_str());
	maxRadiusValue = atoi(configFileRead.readValue("maxRadiusValue").c_str());
	circleLikeValue = atoi(configFileRead.readValue("circleLikeValue").c_str());
	markAreaSize = atoi(configFileRead.readValue("markAreaSize").c_str());
	int ret = configFileRead.closeConfigFile();
	if (ret != 0)
	{
		cout << "config file close failed, program shutdown!" << endl;
		return;
	}


	if (lineWidthValue <= 0 || pointNumValue <= 0)
	{
		cout << "params in config file are not in wright range, program shutdown!" << endl;
		return;
	}

	cout << "read and close configFile success!!!" << endl;

	///start threads
	cap_lo.open(0);
	cap_up.open(2);
	std::thread thread_fetch_frame(fetch_frame, this);
	std::thread thread_lo_process(lo_process, this);
	std::thread thread_up_process(up_process, this);
	thread_fetch_frame.detach();
	thread_lo_process.detach();
	thread_up_process.detach();
}



void VideoProcess::init_base_contour(vector<vector<Point>> & vec, const Mat &frame) {

	Mat src1 = Mat::zeros(frame.size(), frame.type());
	rectangle(src1, Rect(Point(100, 100), Point(frame.rows / 2, frame.rows / 2)), Scalar(255, 255, 255), -1, 8);
	Mat gray1;
	cvtColor(src1, gray1, CV_RGB2GRAY);
	vector<Vec4i> hierarchy1;
	vector<vector<Point>> Basecontour1;
	findContours(gray1, Basecontour1, hierarchy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	vec.push_back(Basecontour1[0]);
}


int VideoProcess::data_process(Mat frame, VideoProcess *videoProcess) {

	/*//test code/// imshow not functional, did get frame.
	//imshow("data Process", frame);
	//cout << "dara process image is :" << frame.rows << "  " << frame.cols << endl;
	//return 0;
	///end test//*/


	//cout<<"-------------letter detect start--------------"<<endl;
	const int classfilternum = 13;
	const string alphabet[classfilternum] = { "A","B","C","D","E","F","G","H","I","J","K","L","M" };
	vector<float>test, test1;

	static int valid_group = 0;

	///Picture Cut///
	Mat detect_src = frame.clone();
	Mat gray;
	int frame_count = 0;
	Mat gauss;
	GaussianBlur(frame, gauss, Size(7, 7), 17);
	addWeighted(frame, 1.5, gauss, -0.5, 0, frame);
	cvtColor(frame, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5, 5), 0, 0);
	//IplImage ipl(gray);
	//int thresh = otsu(&ipl);
	Mat grayInv = gray.clone();
	//threshold(gray, gray, thresh, 255, THRESH_BINARY);
	adaptiveThreshold(gray, gray, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 83, 43);
	bitwise_not(gray, grayInv);

	//Mat grayShow;
	//resize(grayInv, grayShow, Size(400, 300));
	imshow("binary", grayInv);
	waitKey(1);
	vector<Vec4i> hierarchy;
	vector<vector<Point>> Basecontour, Basecontour1;
	vector<Mat> roiSets;
	findContours(grayInv, Basecontour, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	float Area;
	float rectArea;
	if (Basecontour.size() <5)
	{
		cout << "	!!! No contours !!!" << endl;
	}
	for (int i = 0; i < Basecontour.size(); i++)
	{
		RotatedRect rectRotate = minAreaRect(Basecontour[i]);

		Area = contourArea(Basecontour[i]);
		rectArea = rectRotate.size.area();
		Rect rectErect = rectRotate.boundingRect();

		//if ((Area >= 0.9 * rectArea) && (Area < frame.cols*frame.rows * 2 / 3) && (Area > frame.cols*frame.rows / 20 / 20))
		if ((Area < frame.cols*frame.rows / 3 / 3) && (Area > frame.cols*frame.rows / 18 / 18) && (Area > 5 * 10))
		{
			if (rectErect.x < 10 || rectErect.y < 10)
			{
				cout << "rectErect is not correct1!!!!!!!!!!!!!!" << endl;
				continue;
			}
			if (rectErect.width / rectErect.height >= 1.5)
			{
				cout << "the height is not enough!" << endl;
				continue;
			}
			int rightDownPOintX = rectErect.x + rectErect.width;
			int rightDownPOintY = rectErect.y + rectErect.height;

			if (rightDownPOintX >= gray.cols - 10 || rightDownPOintY >= gray.rows - 10)
			{
				cout << "rectErect is not correct2!!!!!!!!!!!!!!" << endl;
				continue;
			}

			Mat imgRotate = gray(rectErect);

			Mat imgRotate2;
			bitwise_not(imgRotate, imgRotate);
			//imshow("imgRotate", imgRotate);
			float rotateAngle = 0;
			if (abs(rectRotate.angle) >= 45)
			{
				if (rectRotate.angle < 0)
				{
					rotateAngle = rectRotate.angle + 90;
				}
				else
				{
					rotateAngle = rectRotate.angle - 90;
				}
			}
			else
			{
				rotateAngle = rectRotate.angle;
			}
			Point2f centerRotate(rectErect.width / 2, rectErect.height / 2);
			Mat matrixAffine = getRotationMatrix2D(centerRotate, rotateAngle, 1);
			warpAffine(imgRotate, imgRotate2, matrixAffine, Size(imgRotate.cols, imgRotate.rows));
			//imshow("imgRotate2", imgRotate2);
			//			bitwise_not(imgRotate,imgRotate);
			vector<Vec4i> hierarchyErrect;
			vector<vector<Point>> contourErrect;
			findContours(imgRotate2, contourErrect, hierarchyErrect, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			float areaBase = 0;
			int maxAreaNUm = -1;
			for (int k = 0; k < contourErrect.size(); k++)
			{
				if (areaBase < contourArea(contourErrect[k]))
				{
					areaBase = contourArea(contourErrect[k]);
					maxAreaNUm = k;
				}
			}
			if (maxAreaNUm < 0)// || areaBase < Area*0.8)
			{
				cout << "	!!! Error - !!!" << endl;
				continue;
			}
			Rect baseRect = boundingRect(contourErrect[maxAreaNUm]);
			if (baseRect.width < 10 || baseRect.height < 10)
			{
				cout << "	!!! Error too small !!!" << endl;
				continue;
			}

			//int cutSizeX = baseRect.width/20;
			//int cutSizeY = baseRect.height/20;
			//Rect rectRoi(baseRect.x + cutSizeX, baseRect.y + cutSizeY, baseRect.width-2*cutSizeX, baseRect.height-2*cutSizeY);

			Rect rectRoi(baseRect.x, baseRect.y, baseRect.width, baseRect.height);
			Mat  imgRoi = imgRotate2(rectRoi);
			bitwise_not(imgRoi, imgRoi);

			if (imgRoi.cols > imgRoi.rows)
			{
				transpose(imgRoi, imgRoi);
				flip(imgRoi, imgRoi, 0);
			}
			//			imshow("imgROI",imgRoi);
						//if(markCheck(imgRoi) < 0)
						//{
							//flip(imgRoi,imgRoi,-1);
							//if(markCheck(imgRoi) < 0)
							//{
								//cout<<"no mark find!!!"<<endl;
								//continue;
							//}
						//}
						//Rect rectRoiCut(0,baseRect.width/10,imgRoi.cols,imgRoi.rows -baseRect.width/5);
			Rect rectRoiCut(0, 0, imgRoi.cols, imgRoi.rows);
			Mat imgRoiCut = imgRoi(rectRoiCut);
			roiSets.push_back(imgRoiCut);
			Basecontour1.push_back(Basecontour[i]);
		}
		else
		{
			//cout << "	!!! Error: Area is not correct !!!" << endl;
		}
	}
	cout<<"gotten roiSets: "<<roiSets.size()<<endl;
	drawContours(detect_src, Basecontour1, -1, Scalar(255, 0, 0), 3);
	vector<Rect>boundRect;
	int detectNum = 0;
	double prob[8];
	char the1stlet = 0;
	char the2ndlet = 0;
	int the1stp = 0;
	int the2ndp = 0;
	if (roiSets.size() <2)
	{
		cout << "	!!! roiSets not enough !!!" << endl;
		return valid_group;
	}
	for (int i = 0; i < roiSets.size(); i++)
	{
		valid_group++;
		vector<Mat> imgRoiTemp;
		vector<double> probTemp;
		double maxProb = 0;
		int maxProbId = 0;
		int rotateNum = 0;
		Mat detectImg = roiSets[i].clone();
		//		bitwise_not(detectImg,detectImg);
		Mat transImg;

		imgRoiTemp.push_back(detectImg);

		Mat dst;
		resize(detectImg, detectImg, Size(IMAGE_ROWS, IMAGE_COLS), (0, 0), (0, 0), INTER_AREA);
		detectImg.convertTo(detectImg, CV_8UC1);
		threshold(detectImg, detectImg, 0, 255, THRESH_BINARY | THRESH_OTSU);
		Mat_<float> testMat(1, IMAGE_ROWS*IMAGE_COLS);
		cout<<"start transforming"<<endl;
		cout<<"size: "<<detectImg.rows<<"  "<<detectImg.cols<<endl;
		for (int ii = 0; ii < 30*30; ii++)
		{
			//cout<<ii<<endl;
			testMat.at<float>(0, ii) = (float)detectImg.at<uchar>(ii / IMAGE_ROWS, ii % IMAGE_COLS);
		}
		cout<<"getting maxProb"<<endl;
		//Predict
		model->predict(testMat, dst);

		cout<<"predict done"<<endl;

		int classId;
		double classProb = 0;
		Point maxLoc;
		minMaxLoc(dst, NULL, &classProb, NULL, &maxLoc);
		classId = maxLoc.x;

		//cout<<"classprob is : "<<j<<" "<<classProb<<endl;
		if (classProb > maxProb)
		{
			maxProb = classProb;
			maxProbId = classId;
			//rotateNum = j;
		}
		probTemp.push_back(classProb);

		//imshow("detect image", detectImg);
		cout << "max classProb is :" << maxProb << endl;
		cout << "max classId is :" << maxProbId << endl;

		putText(detect_src, alphabet[(int)maxProbId], Basecontour1[i][0] + Point(20, 20), 1, 2, Scalar(255, 255, 0), 2, 8);
		//putText(detect_src, classProb*100, Basecontour1[i][0] + Point(10, 0), 1, 4, Scalar(255, 255, 0), 2, 8);
		putText(detect_src, to_string(classProb*100), Basecontour1[i][0] + Point(10, 0), 1, 1, Scalar(255, 0, 0), 2, 8);
		cout<<alphabet[(int)maxProbId]<<"  "<<to_string(classProb*100)<<"%"<<endl;

		detectResult[detectNum] = alphabet[(int)maxProbId].at(0);
		detect_index[detectNum] = (Basecontour1[i][0] + Point(10, 0)).x;
		prob[detectNum] = maxProb;
		detectNum++;
		if (detectNum >= 4)
			break;
	}
	detectFlag = 1;
	//imshow("output",detect_src);
	static int pic_save_id = 0;
	//imwrite("./Letter/detect_" + to_string(valid_group) + ".jpg", detect_src);
	for (int i = 0; i <= detectNum; i++) {
		int temp = 0;
		for (int j = i; j <= detectNum; j++) {
			if (detect_index[j] < detect_index[i]) {
				temp = detect_index[i];
				detect_index[i] = detect_index[j];
				detect_index[j] = temp;
				temp = detectResult[i];
				detectResult[i] = detectResult[j];
				detectResult[j] = temp;
			}
		}
	}
	//if(detectNum >0)
	//{
		//the1stlet = 0;
		//the2ndlet = 0;
		//the1stp = 0;
		//the2ndp = 0;
		//for (int yxj1 = 0; yxj1 < detectNum; yxj1++) {
			//if (prob[yxj1] > the1stp) { the1stp = prob[yxj1]; the1stlet = detectResult[yxj1]; }
			//else if (prob[yxj1] > the2ndp&&prob[yxj1] < the1stp) { the2ndp = prob[yxj1]; the2ndlet = detectResult[yxj1]; }
			//else if (prob[yxj1] == the1stp) { the2ndp = prob[yxj1]; the2ndlet = detectResult[yxj1]; }
		//}
	//}
	//detectResult2[0] = the1stlet;
	//detectResult2[1] = the2ndlet;
	if (detectNum >= 4)
	{
		double maxprob4 = 0;
		double maxprob3 = 0;
		double maxprob2 = 0;
		double maxprob1 = 0;
		for (int tt = 0; tt < 8; tt++)
		{
			if (prob[tt] > maxprob4)
				maxprob4 = prob[tt];
		}
		for (int tt = 0; tt < 8; tt++)
		{
			if (prob[tt] > maxprob3 && prob[tt] < maxprob4)
				maxprob3 = prob[tt];
		}
		for (int tt = 0; tt < 8; tt++)
		{
			if (prob[tt] > maxprob2 && prob[tt] < maxprob3)
				maxprob2 = prob[tt];
		}
		for (int tt = 0; tt < 8; tt++)
		{
			if (prob[tt] > maxprob1 && prob[tt] < maxprob2)
				maxprob1 = prob[tt];
		}

		if (maxprob1 > 0.7 && maxprob2 > 0.7 && maxprob3 > 0.7 && maxprob4 > 0.7)
		{

			//detectFlag = 1;		//ic2letters = 1;
			static int picNO = 0;
			picNO++;
			imwrite("./Letter/detect_high_confidence_" + to_string(picNO) + ".jpg", detect_src);
			//cout << "	Write succeed !" << endl;
			detectResult2[0] = detectResult[0];
			detectResult2[1] = detectResult[1];
			detectResult2[2] = detectResult[2];
			detectResult2[3] = detectResult[3];
			//detectResult2[4] = detectResult[4];
			//detectResult2[5] = detectResult[5];
			//detectResult2[6] = detectResult[6];
			//detectResult2[7] = detectResult[7];
			ofstream fout("./Letter/letter.txt", ios_base::out | ios_base::app);
			fout << detectResult2[0] << detectResult2[1] << detectResult2[2] << detectResult2[3] << endl;
			fout.close();
			detectFlag = 1;
		}
		//ofstream fout(xyl, ios_base::out | ios_base::app);
		//fout << int(detectResult2[0]) << endl << int(detectResult2[1])<<endl;
		//fout.close();
	}
	//else
		//detectFlag = 0;
	resize(detect_src, detect_src, Size(400, 300));
	//imshow("detect_src", detect_src);
	char c = waitKey(1);
	return valid_group;
}

bool obs_(Mat &src)
{
	resize(src, src, Size(src.cols / 2, src.rows / 2));
	Mat bgr;
	inRange(src, Scalar(141, 118, 192), Scalar(156, 208, 251), bgr);
	//imshow("bgr", bgr);
	Mat ero;
	erode(bgr, ero, getStructuringElement(MORPH_RECT, Size(5, 5)));
	//imshow("erode", ero);

	//findContours(ero,)
	vector<Vec4i> hierarchy;
	vector<vector<Point>> cont;
	findContours(ero, cont, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	Rect r;
	int max_id = 0;
	if (cont.size() == 0) { return false; }
	for (int i = 0; i < cont.size(); i++)
	{
		double area = contourArea(cont[i]);
		if (area > contourArea(cont[max_id]))
		{
			max_id = i;
		}
	}
	//if (max_id < 0) { return; }
	r = boundingRect(cont[max_id]);
	rectangle(src, r, Scalar(125, 125, 125), 3);
	Moments M = moments(cont[max_id]);
	Point2f p1 = Point2f(M.m10 / M.m00, M.m01 / M.m00);
	imshow("src", src);
	double rect_area = r.area();
	float ratio = float(r.width) / float(r.height);
	cout << "area: " << r.area() << endl;
	cout << "ratio: " << ratio << endl;
	cout << "x: " << p1.x << "y: " << p1.y << endl;
	static int obsId = 0;
	if (r.area() > src.rows*src.cols/8/10 && p1.y < src.rows / 3 && ratio < 1)
	{
		cout << "           detected!!!" << endl;
		markFlag = 1;
		imwrite("./result/obs"+to_string(obsId)+".jpg",src);
		obsId++;
		return true;
	}

	cout << "---------------" << endl;
	//waitKey(100);
	return false;
}


void VideoProcess::lo_process(VideoProcess *videoProcess)
{

	int fps = 25;
	int wait = 100 / fps;
	Mat image_lo;

	usleep(1000*500);
	while (1)
	//if(false)
	{
		///read one frame
		//usleep(100);
		sem_wait(&sem_lo);
		pthread_mutex_lock(&work_mutex);
		image_lo = videoProcess->frame_lo;
		sem_post(&sem_lo);
		pthread_mutex_unlock(&work_mutex);
		if(image_lo.empty()){
			cout<<"lower frame empty"<<endl;
			continue;
		}

		///put your code here, don't call the imshow function
		//int lineFindState = 0; // static int, LJX
		LinePointFinder pointFinder;
		CircleFinder circleFinder;

		Mat img = image_lo;
		Mat imgResize;

		int width = 0;
		int height = 0;

		Point pointA;
		Point pointB;

		Point centerFind;
		int radiusFind;

		//	double imuAngel = 0;
		double frontAngle = 0;
		double diffAngle = 0;
		double distanceCircle = 0;
		int findLetterFlag = 0;

		Size S(640,480);
		//double fps = videoProcess->cap_lo.get(CAP_PROP_FPS);
		fps = 25;
		/*
		int myFourCC = VideoWriter::fourcc('m', 'p', '4', 'v');
		string name = to_string(time(NULL));
		VideoWriter shipin("./video/" + name + ".avi", myFourCC, 25, S, true);
		if (!shipin.isOpened())
		{
			cout << "VideoWriter open failed!" << endl;
			getchar();
			return;
		}
		else { cout << "VideoWriter open success!" << endl; }
		*/

		/*TIMING CONTROL*/
		int circle_timer = 0;
		int circleColor = GREEN;//0;
		double time0 = static_cast<double>(getTickCount());
		//videoProcess->cap_lo >> img;

		lineFindState = 0;
		findLetterFlag = 0;
		//cout << "lineFindState is :" << lineFindState << endl;

		width = image_lo.cols;
		height = image_lo.rows;

		/// line procedure
		cout<<"-------------line detect start--------------"<<endl;
		resize(img, imgResize, Size(width / 2, height / 2));
		//cout << "resize image size is :" << imgResize.rows << "  " << imgResize.cols << endl;
		Mat imgSegment(imgResize.rows, imgResize.cols, CV_8U);
		pointFinder.setDectParams(lineFindState, lineWidthValue, pointNumValue, segThresholdValue);
		pointFinder.segmentProcess2(imgResize, imgSegment);
		ret = pointFinder.calculatePoint(imgSegment, pointA, pointB);

		if (0 == ret)
		//if (false)
		{
			line(imgResize, pointA, pointB, Scalar(255, 0, 0), 2, 8, 0);
			//line(img, Point(pointA.x * 2, pointA.y * 2), Point(pointB.x * 2, pointB.y * 2), Scalar(255, 0, 0), 2, 8, 0);


			diffAngle = frontAngle - imuAngle;
			if (diffAngle > PI)
				diffAngle = 2 * PI - diffAngle;
			if (diffAngle < -PI)
				diffAngle = 2 * PI + diffAngle;
			diffAngle = abs(diffAngle);
			//cout << "diffAngle is : " << diffAngle << endl;
			//cout << pointA.x << " " << pointA.y << ", " << pointB.x << " " << pointB.y << endl;
			if (diffAngle <= PI / 2)
			{
				pointFinder.calculateResult(imgResize, pointA, pointB);
			}
			else
			{
				pointFinder.calculateResult(imgResize, pointB, pointA);
			}
			resultDistance = pointFinder.distanceResult;
			resultAngle = pointFinder.angleResult;
			resultOutFlag = 1;
			cout << "distance and angle is : " << pointFinder.distanceResult << " , " << pointFinder.angleResult << endl;
		}
		/*TIMING CONTROL*/
		//imshow("line display", imgResize);
		waitKey(5);

		/// circle procedure

		//if (circle_timer < START_CIRCLE_DETECT) // count to start circle detection
		//{
			//circle_timer++; continue;
		//}
		double start2 = (double)getTickCount();
		if (lineFindState < 3)
		{
			/*TIMING CONTROL*/
			cout<<"-------------circle detect start--------------"<<endl;
			cout << "lineFindState: " << lineFindState << endl;

			//if (lineFindState == 1 && circle_timer < 350) { circle_timer++; continue; }
			//if (lineFindState == 2 && circle_timer < 500) { circle_timer++; continue; }
			cout << "circle_timer: " << circle_timer << endl;
			Mat imageSegmentCircle(imgResize.rows, imgResize.cols, CV_8U);
			Mat imgResize2;
			resize(image_lo, imgResize2, Size(width / 2, height / 2)); //ÃÃÂ²ÃÃÃ¹ÃÂªÃÂ­Â³ÃÂ´Ã§ÂµÃ1/2

			circleFinder.setParams(circleColor, minDistValue, minRadiusValue, maxRadiusValue, circleLikeValue, segThresholdValue);
			circleFinder.segmentProcess2(imgResize2, imageSegmentCircle);

			//imshow("circleSegment", imageSegmentCircle);
			ret = circleFinder.circleCalculate2(imageSegmentCircle);
			//ret = circleFinder.circleCalculate(imgResize2);
			if (0 == ret)
			{
				distanceCircle = circleFinder.circleDistance(imgResize2);
				if (distanceCircle <= markAreaSize / 2)
				{
					lineFindState++;

					String imageBaseName = "result/MarkResult_";
					String imageName = imageBaseName + to_string(lineFindState) + ".jpg";
					//imwrite(imageName, image_lo);
					//cout << "mark successd! the mark num is :" << lineFindState << endl;
					//circle(imgResize,circleCenter,radius,Scalar(255),2);
					markFlag = 1;
					markResult[lineFindState - 1] = 0X7F;
				}
			}

		}
		double end2 = (double)getTickCount();
		//cout << "data_processd using time" << (end2 - start2) / getTickFrequency() * 1000 << endl;

		/// obs procedure
		// start obs after circle, irrelevant with letter

		//if (lineFindState >= 3 && !obs_detect_flag)

		{
			cout<<"-------------obs detect start--------------"<<endl;
			obs_detect_flag = obs_(img);
		}

		}

}


void VideoProcess::up_process(VideoProcess *videoProcess) {
	int fps = 25;
	int wait = 100 / fps;
	Mat image_up;
	usleep(1000 * 500);
	//bool saveOne = true;

	usleep(20000000);
	while (1) {
		//read one frame
		//usleep(1000);

		sem_wait(&sem_up);
		pthread_mutex_lock(&work_mutex);
		image_up = videoProcess->frame_up;
		sem_post(&sem_up);
		pthread_mutex_unlock(&work_mutex);

		if(image_up.empty()){
			cout<<"upper frame empty"<<endl;
			continue;
		}

		//put your code here, don't call the imshow function

		//double time0 = static_cast<double>(getTickCount());
		//cout << "frame size is :" << image_up.rows << " " << image_up.cols << endl;

		if(/*lineFindState == 3 &&*/!letter_detect_flag)
		{
			//shipin.write(image_up);

			double start3 = (double)getTickCount();
			videoProcess->data_process(image_up, videoProcess);
			double end3 = (double)getTickCount();
			cout << "data_processd using time" << (end3 - start3) / getTickFrequency() * 1000 << endl;
			//		    resize(img, imgResize,Size(640,480));
			//findLetterFlag = 1;
		}
	}
}

int VideoProcess::otsu(IplImage* src)
{
	int hist_size = 256;
	int hist_height = 256;
	float range[] = { 0,255 };
	float* ranges[] = { range };
	CvHistogram* gray_hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	cvCalcHist(&src, gray_hist, 0, 0);
	cvNormalizeHist(gray_hist, 1.0);

	int Width = src->width;
	int Height = src->height;
	int threshold = 0;
	double delta = 0;
	double U_t = 0;
	for (int m = 0; m < 256; m++)
	{
		U_t += cvQueryHistValue_1D(gray_hist, m)*m;
	}
	double u = 0, w = 0;
	for (int k = 0; k < 256; k++)
	{
		u += cvQueryHistValue_1D(gray_hist, k)*k;
		w += cvQueryHistValue_1D(gray_hist, k);

		double t = U_t * w - u;
		double delta_tmp = t * t / (w * (1 - w));

		if (delta_tmp > delta)
		{
			delta = delta_tmp;
			threshold = k;
		}
	}
	cvReleaseHist(&gray_hist);
	return threshold;
}

void VideoProcess::getMaxClass(const Mat &probBlob, int *classId, double *classProb)
{
	Mat probMat = probBlob.reshape(1, 1);
	Point classNumber;
	minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
	*classId = classNumber.x;
}
