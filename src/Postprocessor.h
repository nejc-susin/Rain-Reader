/*
* Postprocessor.h
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#ifndef POSTPROCESSOR_H_
#define POSTPROCESSOR_H_

#include "PointList.h"
#include "Data.h"

#include <vector>


class Postprocessor {
public:
	Postprocessor();
	virtual ~Postprocessor();
	bool apply(PointList points, std::vector<PointList> &result, std::vector<int> inversionPoints, Data data);
private:
	void smooth(PointList points, PointList inversions, std::vector<PointList> &result);
	int lis(PointList points, int index, cv::Point endpoint, PointList &result);
};

#endif /* POSTPROCESSOR_H_ */
