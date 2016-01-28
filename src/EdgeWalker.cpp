/*
* EdgeWalker.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "EdgeWalker.h"


#define BRANCH_FACTOR 5

EdgeWalker::EdgeWalker() {
	depth = 5;
}

EdgeWalker::EdgeWalker(cv::Mat ch) {
	depth = 5;
	channel = ch;
	size[0] = channel.size().height;
	size[1] = channel.size().width;
}

EdgeWalker::EdgeWalker(cv::Mat ch, unsigned int d) {
	depth = d;
	channel = ch;
	size[0] = channel.size().height;
	size[1] = channel.size().width;
}

EdgeWalker::EdgeWalker(unsigned int d) {
	depth = d;
}


EdgeWalker::~EdgeWalker() {

}

bool EdgeWalker::validPoint(cv::Point p, cv::Point size) {
	return p.x > 0 && p.y > 0 && p.x<size.x && p.y<size.y;
}


void EdgeWalker::use(cv::Mat ch) {
	channel = ch;
	size[0] = channel.size().height;
	size[1] = channel.size().width;
	points.clear();
	queue.clear();
}

void EdgeWalker::setDepth(unsigned int d){
	if (d > 0)depth = d;
}


bool EdgeWalker::walk(cv::Point start, PointList& out){
	queue.clear();
	points = out;
	visited.create(2, size, CV_8UC1);
	if (out.empty())return false;
	if (channel.at<uchar>(start) < 240)return false;
	points.push_back(start);
	queue.push_back(0);

	//cout << "<EdgeWalker> starting walking(PointList).."<<start<<endl;
	while (true){
		//cout << "<EdgeWalker> \t\tqueue.size ="<<queue.size()<<endl;
		if (queue.empty())break;
		while (step());
		if (!backtrack())break;
	}
	//cout << "<EdgeWalker> \t\tclensing.."<<endl;
	visited.create(2, size, CV_8UC1);
	queue.clear();
	queue.push_back(0);
	while (!queue.empty() && step());
	out = points;
	//cout << "<EdgeWalker> DONE"<<endl;
	return true;
}

bool EdgeWalker::walk(cv::Point start, unsigned int length){
	queue.clear();
	points.clear();
	visited.clear();

	visited.create(2, size, CV_8UC1);
	points.push_back(start);
	queue.push_back(0);
	//cout << "<EdgeWalker> starting walking(length).."<<start<<endl;
	while (true){
		//////cout << "queue size="<<queue.size()<<endl;
		if (queue.empty())break;
		while (step())
			if (points.size() >= length)return true;
		if (!backtrack())break;
	}
	queue.clear();
	points.clear();
	visited.create(2, size, CV_8UC1);
	return false;


}

bool EdgeWalker::backtrack() {

	while (!queue.empty()) {
		// if other steps from current point possible - try next untried
		if (queue.back() < BRANCH_FACTOR - 1){
			queue.back()++;
			return true;
		}
		else{
			//all options from this point exhausted - do not visit again
			visited.ref<uchar>(points.back().y, points.back().x) = 255;
			queue.pop_back();
			points.pop_back();
		}
	}

	return false;
}



bool EdgeWalker::step() {
	//cout << "<EdgeWalker> \tstep:\t"<<endl;
	int number = queue.back();
	if (number > BRANCH_FACTOR - 1 || visited.ref<uchar>(points.back().y, points.back().x)){
		return false;
	}
	PointList::reverse_iterator it;
	bool skip;
	unsigned int count;
	cv::Point next;
	//try all directions until success
	for (int i = number; i < BRANCH_FACTOR; i++){
		//cout << "<EdgeWalker> \tstep:\t"<<"queue.size="<<queue.size()<<"\tpoints.size="<<points.size()<<"\t"<<endl;
		//step is possible
		if (checkStep(points.back(), next, i)){
			// check:
			// no cycles
			skip = false;
			for (it = points.rbegin(), count = 1; it != points.rend() && count < BRANCH_FACTOR*depth; count++, ++it) {
				if (next == *it){
					skip = true;
					break;
				}
			}
			//cout << "<EdgeWalker> \tskip="<<skip<<"\tcurrent="<<points.back()<<"\tnext="<<next<<endl;
			// not already visited
			if (visited.ref<int>(next.y, next.x) || skip) continue;
			////cout << "step "<< points.back() << next << i <<endl;
			queue.back() = i;
			points.push_back(next);
			queue.push_back(0);
			if (queue.size() > depth)
				queue.pop_front();
			return true;
		}
	}
	//cout << "<EdgeWalker> \tstep:\tFALSE"<<endl;
	return false;
}

bool EdgeWalker::checkStep(cv::Point start, cv::Point& next, int index){
	cv::Point result;
	//cout << "<EdgeWalker> \t\tcheck step:\t"<<start;
	switch (index){
	case 0:
		//DOWN
		result.y = start.y + 1;
		result.x = start.x;
		//cout << "\td\t";
		break;
	case 1:
		//DOWN-RIGHT
		result.y = start.y + 1;
		result.x = start.x + 1;
		//cout << "\tdr\t";
		break;
	case 2:
		//RIGHT
		result.y = start.y;
		result.x = start.x + 1;
		//cout << "\tr\t";
		break;
	case 3:
		//UP-RIGHT
		result.y = start.y - 1;
		result.x = start.x + 1;
		//cout << "\tur\t";
		break;
	case 4:
		//UP
		result.y = start.y - 1;
		result.x = start.x;
		//cout << "\tu\t";
		break;

	default:
		//cout << "default -- SHOULD NOT HAPPEN"<<endl;
		return false;

	}
	if (validPoint(result, channel.size()) && channel.at<uchar>(result) > 240){
		next = result;
		//cout << "OK"<<endl;
		return true;
	}
	//cout << "NOPE"<<endl;
	return false;


}

