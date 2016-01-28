/*
* AverageWalker.h
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#ifndef AVERAGEWALKER_H_
#define AVERAGEWALKER_H_

#include "SimpleHistogram.h"
#include "PointList.h"

using namespace std;

class AverageWalker{
public:
	AverageWalker(int windowWidth);
	virtual ~AverageWalker();
	void use(const cv::Mat ch, const cv::Rect rect);
	bool walk(cv::Point& start, PointList& list);
	bool step(cv::Point& start, cv::Point& next);
private:
	cv::Rect aoi;
	cv::Mat ch;
	float evidence;
	cv::Point current;
	int windowWidth;
	int half;
};


#endif /* AVERAGEWALKER_H_ */
