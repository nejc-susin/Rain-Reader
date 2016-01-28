/*
* Data.h
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#ifndef CALIBRATOR_H_
#define CALIBRATOR_H_

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "SimpleHistogram.h"
#include "EdgeWalker.h"

class Data{
public:
	Data();
	virtual ~Data();
	bool apply(cv::Mat b);
	cv::Rect getInnerRectangle();
	cv::Rect getOuterRectangle();
	cv::Rect getAreaOfInterest();
	cv::Rect getLowerRectangle();
	cv::Rect getUpperRectangle();
	cv::Rect getSearchL();
	cv::Rect getSearchR();
	cv::Rect getSearchT();
	cv::Rect getSearchB();
	void setAoi(int tlx, int tly, int brx, int bry);
	void setAoi(cv::Rect newAoi);
private:
	cv::Rect outerRect, innerRect, aoi, searchL, searchT, searchR, searchB;
	int calculate(cv::Mat ch, bool vertical);
};

#endif /* CALIBRATOR_H_ */
