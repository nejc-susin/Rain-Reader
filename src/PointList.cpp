/*
* PointList.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "PointList.h"

PointList::PointList() {
	// TODO Auto-generated constructor stub

}

PointList::~PointList() {
	// TODO Auto-generated destructor stub
}

void PointList::flattenX() {
	iterator previous;
	iterator current = begin();
	if (current == end())
		return;
	previous = current;
	++current;
	while (current != end()) {
		int sum = 0;
		int count = 0;
		if ((*previous).x == (*current).x) {
			while (current != end() && (*previous).x == (*current).x){
				sum += (*current).x;
				count++;
				++current;
			}
			--current;
			current = erase(previous, current);
			(*current).x = sum / count;
		}
		previous = current;
		++current;
	}
}

void PointList::sort()
{
	struct {
		bool operator()(cv::Point a, cv::Point b)
		{
			return a.x < b.x;
		}
	} sorter;
	std::sort (begin(), end(), sorter);
}

