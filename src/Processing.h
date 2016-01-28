/*
* Processing.h
*
*	Created in 2015
*		By: Nejc Susin
*/

#ifndef PROCESSING_H
#define PROCESSING_H

#include <algorithm>
#include <opencv2\core\core.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/date_facet.hpp"

#include "RegionGrowingThresholder.h"
#include "CurveExtractor.h"
#include "Data.h"
#include "Postprocessor.h"
#include "Converter.h"

class Processing {
public:

	Processing();
	virtual ~Processing();

	void setImage(cv::Mat input);
	void threshold(int a1, int a2, int b1, int b2);
	void extract();
	void postProcess(int resolution);
	string results(int resolution, string datestr, int timespan);
	string entireProcess(int resolution, string datestr, int timespan, int thresh[4]);

	bool addExclusion(cv::Rect rect);
	void removeExclusion(int idx);
	void addInclusion(std::vector<cv::Point> path);
	void removeInclusion(int idx);
	void addInversion(int x);
	void removeInversion(int x);
	void applyCorrections();
	void resetCorrections();

	void makeGrid(int timespan);
	void segmentGrid();

	cv::Mat getOriginal();
	cv::Mat getChannel(int ch);
	cv::Mat getBinary(int ch);
	cv::Mat getAoi(int aoi, int grid, int border, int search);
	cv::Mat getExtracted(int back, int exclIdx, int inclIdx, int invsIdx);
	cv::Mat getFinal();
	cv::Mat getDebug();


	cv::Rect getAoiRect();
	void setAoi(cv::Rect newAoi);

private:

	Data data;
	RegionGrowingThresholder thresholder;
	Postprocessor postProcessor;
	Converter converter;

	cv::Mat image;
	cv::Mat channels[3];
	cv::Mat binary[3];
	cv::Mat corrected;
	cv::Mat correctedDisplay;

	std::vector<cv::Rect> exclusions;
	std::vector<std::vector<cv::Point>> inclusions;
	std::vector<int> inversionPoints;

	std::vector<int> gridPointsX;
	std::vector<int> gridPointsY;

	PointList points;
	std::vector<PointList> segments;
};

#endif /* PROCESSING_H_ */