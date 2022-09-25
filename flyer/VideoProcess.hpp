/* ************************************************************************
 *       Filename:  VideoProcess.hpp
 *    Description:
 *        Version:  1.0
 *        Created:  13/03/19 03:59:03
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  YOUR NAME (),
 *        Company:
 * ************************************************************************/
#ifndef __PROCESS__
#define __PROCESS__

#include "common.h"
#include "queue.h"

class VideoProcess {
private:
	dnn::Net net;
	Ptr<ANN_MLP> model;
	VideoCapture cap_lo;
	VideoCapture cap_up;
	Mat frame_lo;
	Mat frame_up;
	int first_flag;
	int frame_count;

	vector<vector<Point>>basic_model;
public:
	VideoProcess();
	void run(); // read configuration and start threads

private:
	static void lo_process(VideoProcess *videoProcess); // lower camera frame process
	static void up_process(VideoProcess *videoProcess); // upper
	static void fetch_frame(VideoProcess *videoProcess); // fething frames from lower and upper camera
	void init_base_contour(vector<vector<Point>> & vec, const Mat &frame);
	int data_process(Mat frame, VideoProcess *videoProcess); // letters recognition algorithm
	int  otsu(IplImage* src); // adaptive binary threshhold calculation
	void getMaxClass(const Mat &probBlob, int *classId, double *classProb);
};

#endif
