/*
* CurveExtractor.h
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#ifndef CURVEEXTRACTOR_H_
#define CURVEEXTRACTOR_H_

#include "AverageWalker.h"
#include "EdgeWalker.h"

#include <stdint.h>


class CurveExtractor{
	public:
		bool apply(const cv::Mat src,PointList& dest);
		void setParameters(const cv::Rect out, const cv::Rect in, const cv::Rect aoi);
		CurveExtractor(int winWidth);
	private:
		bool findStart(cv::Mat ch, cv::Point& start);
		cv::Rect outer, inner, area;
		int windowWidth;
};


#endif /* CURVEEXTRACTOR_H_ */




