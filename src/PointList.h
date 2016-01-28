/*
 * PointList.h
 *
 *  Created on: Nov 4, 2008
 *      Author: Gasper Derganc
 *	Updated in 2015
 *		By: Nejc Susin
 */

#ifndef POINTLIST_H_
#define POINTLIST_H_

#include "opencv2/core/core.hpp"

#include <deque>

using namespace std;

class PointList : public std::deque<cv::Point>{
public:
	PointList();
	virtual ~PointList();
	void flattenX();
	void sort();
};

#endif /* POINTLIST_H_ */
