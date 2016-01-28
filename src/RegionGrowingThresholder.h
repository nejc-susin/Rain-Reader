/*
* RegionGrowingThresholder.h
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#ifndef RGROWTHR_H_
#define RGROWTHR_H_

#include "opencv2/core/core.hpp"

class RegionGrowingThresholder{

	public:
		RegionGrowingThresholder();
		void apply(cv::Mat in, cv::Mat& out, int upperThreshold, int lowerThreshold);
};

#endif
