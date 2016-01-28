/*
* CurveExtractor.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "CurveExtractor.h"
#include <iostream>

CurveExtractor::CurveExtractor(int winWidth) {
	windowWidth = winWidth;
}

void CurveExtractor::setParameters(const cv::Rect out, const cv::Rect in, const cv::Rect aoi){
	outer = out;
	inner = in;
	area = aoi;
}

bool CurveExtractor::findStart(const cv::Mat ch, cv::Point& start){
	EdgeWalker walker(10);
	walker.use(ch);
	cv::Point p;
	for (p.y = outer.br().y; p.y > outer.y; p.y--){
		for (p.x = outer.x; p.x < inner.x; p.x++){
			if (ch.at<uchar>(p) > 240 && walker.walk(p, 100)){
				start = p;
				return true;
			}
		}
	}
	return false;
}

bool CurveExtractor::apply(const cv::Mat src, PointList& dest) {
	PointList list;
	AverageWalker averager(windowWidth);
	float sumavg = 0, sumwalk = 0;
	int startcount, endcount;
	averager.use(src, area);
	EdgeWalker walker(10);
	walker.use(src);
	cv::Point start;
	bool startfound = false;
	//std::cout<<"<CurveExtractor> finding start.."<<endl;
	startfound = findStart(src, start);
	if (startfound){
		//std::cout<<"\tfound:"<<start<<endl;
		dest.push_back(start);
	}
	else{
		//std::cout<<"\tNOT FOUND"<<endl;
		if (!averager.walk(area.tl(), dest)) return false;
	}
	bool success = true;
	while (success) {
		//TEST
		//startcount = dest.size();
		//ENDTEST

		walker.walk(dest.back(), dest);

		//TEST
		/*endcount = dest.size();
		sumwalk+=endcount-startcount;
		cout << "walked "<< endcount-startcount << " steps."<<endl;
		startcount = dest.size();*/
		//ENDTEST

		success = averager.walk(dest.back(), dest);

		//TEST
		/*endcount = dest.size();
		cout << "averaged "<< endcount-startcount << " steps."<<endl;
		sumavg+=endcount-startcount;*/
		//ENDTEST

	}
	// one pixel per column
	dest.flattenX();
	//std::cout<<"[INFO] walking/averaging ratio="<<sumwalk << " / "<<sumavg<<endl;
	return true;
}
