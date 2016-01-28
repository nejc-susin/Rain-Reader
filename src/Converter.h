/*
* Converter.h
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "PointList.h"
#include "Data.h"

#include <vector>
#include <cmath>

using namespace std;


class Converter {
public:
	Converter();

	virtual ~Converter();
	void apply(std::vector<PointList> segments, std::vector<float> &result, Data data, int minutes, int timespan);
};

#endif /* CONVERTER_H_ */
