/*
* Data.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "Data.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
Data::Data() {

}

Data::~Data() {

}

cv::Rect Data::getInnerRectangle() {
	return innerRect;
}
cv::Rect Data::getOuterRectangle() {
	return outerRect;
}
cv::Rect Data::getAreaOfInterest() {
	return aoi;
}
void Data::setAoi(cv::Rect newAoi) {
	aoi = newAoi;
}
cv::Rect Data::getSearchL() {
	return searchL;
}
cv::Rect Data::getSearchR() {
	return searchR;
}
cv::Rect Data::getSearchT() {
	return searchT;
}
cv::Rect Data::getSearchB() {
	return searchB;
}
cv::Rect Data::getLowerRectangle() {
	return cv::Rect(outerRect.tl().x, innerRect.br().y, outerRect.br().x,
			outerRect.br().y);
}
cv::Rect Data::getUpperRectangle() {
	return cv::Rect(outerRect.tl().x, outerRect.tl().y, outerRect.br().x,
			innerRect.tl().y);
}

bool Data::apply(cv::Mat ch){
	cv::Mat binImg = ch.clone();
	cv::threshold(ch, binImg, 145.0, 255.0, cv::THRESH_TOZERO_INV);

	bool ok = true;
	// predefined relative rectangles used for narrowing the search of graph borders
	// outer rectangle
	outerRect = cv::Rect(	static_cast<int>(0.015f*ch.size().width), static_cast<int>( 0.050f*ch.size().height),
							static_cast<int>(0.945f*ch.size().width), static_cast<int>( 0.940f*ch.size().height));
	// inner rectangle
	innerRect = cv::Rect(	static_cast<int>(0.053f*ch.size().width), static_cast<int>( 0.120f*ch.size().height),
							static_cast<int>(0.867f*ch.size().width), static_cast<int>( 0.800f*ch.size().height));
	cv::Mat part;
	cv::Rect crop;
	int tlx, tly, brx, bry;

	//
	// find start line   | --v--
	//
	searchL = cv::Rect(outerRect.x, outerRect.y, innerRect.x - outerRect.x, outerRect.height);
	part = binImg.clone();
	part = part(searchL);
	tlx = calculate(part, true);
	if( tlx != -1)
		tlx += outerRect.x;
	else ok = false;

	//
	// find stop line      --v-- |
	//
	searchR = cv::Rect(innerRect.br().x, outerRect.y, outerRect.br().x - innerRect.br().x, outerRect.height);
	part = binImg.clone();
	part = part(searchR);
	if( (brx = calculate(part, true)) != -1)
		brx += innerRect.br().x;
	else ok = false;
	//                      ---
	// find top line       --v--
	//
	searchT = cv::Rect(outerRect.x, outerRect.y, outerRect.width, innerRect.y - outerRect.y);
	part = binImg.clone();
	part = part(searchT);
	if ((tly = calculate(part, false)) != -1)
	{
		tly += outerRect.y;
		int a;
	}
	else ok = false;
	//
	// find bottom line    --v--
	//                      ---
	searchB = cv::Rect(outerRect.x, innerRect.br().y, outerRect.width, outerRect.br().y - innerRect.br().y);
	part = binImg.clone();
	part = part(searchB);
	if( (bry = calculate(part, false)) != -1)
		bry += innerRect.br().y;
	else ok = false;

	aoi = cv::Rect(tlx, tly, brx-tlx, bry-tly);
	

	// DEBUG - show AOI
	/*cv::rectangle(binImg, aoi, cv::Scalar(255, 0, 0), 2, 8);
	cv::namedWindow("aoi", CV_WINDOW_NORMAL);
	cv::imshow("aoi", binImg);
	cv::waitKey(0);
	*/

	return ok;
}


int Data::calculate(cv::Mat ch, bool vertical){
	int result = -1;
	cv::Point p;
	SimpleHistogram hist(vertical ? ch.size().width : ch.size().height);
	for (p.y = 0; p.y < ch.size().height; p.y++){
		for (p.x = 0; p.x < ch.size().width; p.x++) {
			if (ch.at<uchar>(p) > 0)
				hist.add(vertical ? p.x : p.y);
		}
	}
	result = hist.indexOfMax();
	return result;
}
