/*
* AverageWalker.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "AverageWalker.h"

using namespace std;

AverageWalker::AverageWalker(int width){
	windowWidth = width;
	half = static_cast<int> (float(windowWidth) / 2.0f);
}

AverageWalker::~AverageWalker() {

}

void AverageWalker::use(const cv::Mat img, cv::Rect rect){
	ch = img;
	aoi = rect;
}

bool AverageWalker::walk(cv::Point& start, PointList& dest) {

	int avg = 0;
	int windowHeight = 0;
	float evidence = 0;
	SimpleHistogram hist(ch.rows);
	cv::Point p(start.x - half, 0);
	if (start.x >= ch.cols) return false;
	// fill histogram
	for (int count = 0; count++ < windowWidth; p.x++) {
		if (p.x >= ch.cols) return false;
		for (p.y = 0; p.y < ch.rows; p.y++) {
			if (ch.at<uchar>(p) >= 240){
				hist.add(p.y);
			}
		}
	}

	// global averaging
	avg = hist.average();
	// if any points in window try with a lower window
	if (avg != -1){
		//windowHeight = lti::max( windowWidth,(7*windowWidth*windowWidth)/(hist.getNumberOfEntries()));
		windowHeight = max(windowWidth, (hist.getNumberOfEntries()) / (windowWidth));
		//cout<<windowHeight<<std::endl;
		avg = hist.average(static_cast<int>(avg - windowHeight), static_cast<int>(avg + windowHeight));

		if (avg != -1){
			dest.push_back(cv::Point(p.x - half, avg));
		}
	}

	for (; p.x < ch.cols; p.x++) {
		for (p.y = 0; p.y < ch.rows; p.y++) {
			if (ch.at<uchar>(p) >= 240){
				hist.add(p.y);
			}
			if (ch.at<uchar>(p.y, p.x - windowWidth) > 0){
				hist.take(p.y);
			}
		}
		avg = hist.average();
		// if any points in window try with a lower window
		if (avg != -1){
			windowHeight = max(windowWidth, (hist.getNumberOfEntries()) / (windowWidth));

			avg = hist.average(static_cast<int>(avg - windowHeight), static_cast<int>(avg + windowHeight));

			if (avg != -1){
				cv::Point newPoint(p.x - half, avg);
				evidence = static_cast<float>(hist.getLastCount()) / static_cast<float>(hist.getNumberOfEntries());
				if (evidence > 0.6)
					dest.push_back(newPoint);

				//cout<<"point:"<<newPoint<<"\theight:"<<windowHeight<<"\tval:"<<uint(ch.at(newPoint))<<"\te:"<<evidence<<std::endl;
				if (evidence > 0.8 && int(ch.at<uchar>(newPoint)) > 240){
					// find pixel on curve
					//cout << "out"<<endl;
					return true;
				}
			}
		}
	}
	return false;
}

