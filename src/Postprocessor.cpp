/*
* Postprocessor.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "Postprocessor.h"

Postprocessor::Postprocessor() {
	// TODO Auto-generated constructor stub

}

Postprocessor::~Postprocessor() {
	// TODO Auto-generated destructor stub
}

// calculates the longest increasing subsequence
int Postprocessor::lis(PointList points, int index, cv::Point endpoint, PointList &result) {
	int len = 1;
	int end = 0;
	std::vector<int> prev;
	std::vector<int> l;
	int j, i = 0;
	for (i = 0; i + index < points.size() && points[i + index].x <= endpoint.x; i++) {
		l.push_back(1);
		prev.push_back(-1);
		for (j = 0; j < i; j++) {
			if (points[j + index].y >= points[i + index].y && l[i] < l[j] + 1) {
				l[i] = l[j] + 1;
				prev[i] = j;
			}
		}
		if (len < l[i]) {
			len = l[i];
			end = i;
		}

	}
	j = end;
	while (prev[j] != -1) {
		result.push_front(points[j + index]);
		j = prev[j];
	}
	if (index + i < points.size())
		return index + i;
	return -1;

}

void Postprocessor::smooth(PointList points, PointList inversions, std::vector<PointList> &result){
	PointList::iterator it;
	int index = 0;
	for (it = inversions.begin(); it != inversions.end(); ++it){
		PointList part;
		index = lis(points, index, *it, part);
		result.push_back(part);
	}
	PointList part;
	index = lis(points, index, cv::Point(10000, 10000), part);
	result.push_back(part);
}


bool Postprocessor::apply(PointList points, std::vector<PointList> &result, std::vector<int> inversionPoints, Data data){
	PointList::iterator it = points.begin();
	PointList::iterator current, previous, med;
	PointList inversions;
	std::vector<int>::iterator invit;

	//	SPOT REJECTION
	current = points.begin();
	previous = current;
	++current;
	int dist1, dist2;
	bool found = false;
	while (current != points.end()) {
		// found
		found = false;
		dist1 = previous->y - current->y;
		if (abs(dist1) > (data.getAreaOfInterest().height) / 3){
			int count = 150;
			med = current;
			++current;
			while (current != points.end() && --count > 0){
				dist2 = med->y - current->y;
				if (abs(dist2) > (data.getAreaOfInterest().height) / 4 && dist1 + dist2 > 0){
					found = true;
					break;
				}
				med = current;
				++current;

			}
			if (found || current == points.end()){
				current = points.erase(previous, current);
			}

		}
		previous = current;
		if (current != points.end())
			++current;
	}

	//  ADD MANUAL INVERSIONS
	current = points.begin();
	invit = inversionPoints.begin();
	previous = current;
	++current;

	while (current != points.end())
	{
		if (invit != inversionPoints.end() && (*current).x > *invit)
		{
			cv::Point temp = cv::Point(*invit, data.getAreaOfInterest().tl().y);
			if ((*previous).x == *invit)
			{
				*previous = temp;
			}
			else
			{
				current = points.insert(current, temp);
				previous = current;
				--previous;
			}
			++invit;
		}
		previous = current;
		++current;
	}

	//	FIND INVERSION POINTS
	int count;
	current = points.begin();

	while (true) {
		previous = current;
		++current;
		if (current == points.end())
		{
			break;
		}
		
		// came to a local minimum - previous lies in the upper third and is higher than current -> previous is the minimum
		if ((*previous).y < (*current).y && (*previous).y < (data.getAreaOfInterest().tl().y + data.getAreaOfInterest().height / 3)) {
			it = current;
			count = 20;
			int timer = 5;
		
			// try to find the maximum
			//	it iterates over potential new maximums
			//	previous points to the minimum
			//	current will point to maximum
			while (it + 1 != points.end() && count > 0) {
				// found new minimum - end search, this is not an inversion.
				if ((*it).y < (*previous).y) {
					count = 20;
					current = it;
					--current;
					break;
				}

				// found new maximum
				if ((*it).y >= (*current).y) {
					current = it;
					timer = 5;
				}
				count--;
				timer--;
				++it;
				// if the curve is stagnating  || ((*current).y - (*it).y) > 10
				if (timer == 0){
					count = 0;
					break;
				}
			}

			// was an inversion actually found?
			if ((count == 0 || it == points.end()) && (*current).y - (*previous).y > (data.getAreaOfInterest().height)*0.5) {
				inversions.push_back((*previous)); 
				it = previous;
				++it;
				current = points.erase(it, current);	// points inbetween are irrelevant - erase them
			}

		}

	}

	//	SMOOTH USING LONGEST INCREASING SUBSEQUENCE
	std::vector<PointList> segments;
	smooth(points, inversions, segments);


	//	FILL IN MISSING VALUES
	// don't know if this is really necessary
	// cutoff beginning
	while (segments[0].front().x < data.getAreaOfInterest().tl().x)
		segments[0].pop_front();
	// fill beginning
	while (segments[0].front().x > data.getAreaOfInterest().tl().x)
		segments[0].push_front(cv::Point(segments[0].front().x - 1, segments[0].front().y));
	int n = segments.size() - 1;
	
	
	// cutoff ending
	/*while (segments[n].back().x > data.getAreaOfInterest().br().x)
	{
		// if last segment is empty
		if (segments[n].size() <= 1)
		{
			segments.pop_back();
			n -= 1;
		}
		segments[n].pop_back();
	}*/
	// fill ending
	while (segments[n].back().x < data.getAreaOfInterest().br().x)
		segments[n].push_back(cv::Point(segments[n].back().x + 1, segments[n].back().y));


	cv::Point t0, t1;
	float k = 0;
	for (int i = 0; i < segments.size(); i++) {
		// interpolation
		//y = y0+(x-x0)(y1-y0)/(x1-x0)
		current = (segments[i]).begin();
		if (current == (segments[i]).end())
			continue;
		previous = current;
		++current;
		while (current != (segments[i]).end()) {
			if (previous->x + 1 < current->x) {
				t0 = *previous;
				k = static_cast<float> ((*current).y - (*previous).y)
					/ static_cast<float> ((*current).x - (*previous).x);
				for (int x = t0.x + 1; x < (*current).x; x++) {
					current = (segments[i]).insert(current, cv::Point(x, static_cast<int> (t0.y + (x - t0.x) * k)));
				}
			}
			previous = current;
			++current;
		}

		// fill inversion gaps
		if ((i + 1) < segments.size()){
			t0 = (segments[i]).back();
			current = segments[i + 1].begin();
			// this would be current point if there were no inversions
			t1.x = current->x;
			t1.y = current->y - data.getAreaOfInterest().height;
			k = static_cast<float> (t1.y - t0.y)
				/ static_cast<float> (t1.x - t0.x);
			// interpolate and add additional point representing the maximum
			// so it will not be required to handle this case in next step
			for (int x = t0.x + 1; x <= t1.x; x++) {
				(segments[i]).push_back(cv::Point(x, static_cast<int> (t0.y + (x - t0.x) * k)));
			}
		}
	}
	if (points.size() > 0) {}
	result = segments;
	return true;

}


