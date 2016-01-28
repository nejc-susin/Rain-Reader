/*
* EdgeWalker.h
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#ifndef EDGEWALKER_H_
#define EDGEWALKER_H_

#include "opencv2/core/core.hpp"

#include "PointList.h"

#include <deque>
#include <iostream>

using namespace std;


class EdgeWalker{
public:
	EdgeWalker();
	EdgeWalker(cv::Mat ch);
	EdgeWalker(cv::Mat, unsigned int depth);
	EdgeWalker(unsigned int depth);
	virtual ~EdgeWalker();
	void use(cv::Mat ch);
	void setDepth(unsigned int d);
	bool walk(cv::Point start, PointList& path);
	bool walk(cv::Point start, unsigned int length);
private:
	bool validPoint(cv::Point p, cv::Point size);
	bool backtrack();
	cv::SparseMat visited;
	void clense();
	bool step();
	bool checkStep(cv::Point start, cv::Point& next, int index);
	unsigned int depth;
	deque<int> queue;
	PointList points;
	cv::Mat channel;
	cv::Size cvSize;
	int size[2];
};

#endif /* EDGEWALKER_H_ */
