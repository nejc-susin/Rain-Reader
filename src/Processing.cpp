/*
* Processing.cpp
*
*	Created in 2015
*		By: Nejc Susin
*/

#include <Processing.h>


CurveExtractor extr(7);

Processing::Processing()
{
}
Processing::~Processing()
{
}
cv::Mat Processing::getOriginal()
{
	return image;
}
cv::Mat Processing::getChannel(int ch)
{
	return channels[ch];
}
cv::Mat Processing::getBinary(int ch)
{
	return binary[ch];
}
cv::Mat Processing::getAoi(int aoi, int grid, int border, int search)	//arguments set to 1 are included - AOI, grid, predefined border and areas where we look for aoi
{
	cv::Mat out = image.clone();
	if (border)
	{
		cv::rectangle(out, data.getOuterRectangle(), cv::Scalar(0, 0, 255), 4, 8);
		cv::rectangle(out, data.getInnerRectangle(), cv::Scalar(0, 0, 255), 4, 8);
	}
	if (search)
	{
		cv::rectangle(out, data.getSearchL(), cv::Scalar(0, 0, 255), 8, 8);
		cv::rectangle(out, data.getSearchR(), cv::Scalar(0, 0, 255), 8, 8);
		cv::rectangle(out, data.getSearchT(), cv::Scalar(255, 125, 0), 8, 8);
		cv::rectangle(out, data.getSearchB(), cv::Scalar(255, 125, 0), 8, 8);
	}
	if (grid)
	{
		for (unsigned i = 0; i < gridPointsX.size(); ++i)
		{
			cv::line(out, cv::Point(gridPointsX[i], 0), cv::Point(gridPointsX[i], out.rows), cv::Scalar(0, 140, 255), 3, 8, 0);
		}
		for (unsigned i = 0; i < gridPointsY.size(); ++i)
		{
			cv::line(out, cv::Point(0, gridPointsY[i]), cv::Point(out.cols, gridPointsY[i]), cv::Scalar(0, 140, 255), 3, 8, 0);
		}
	}
	if (aoi)
	{
		cv::rectangle(out, data.getAreaOfInterest(), cv::Scalar(255, 0, 0), 4, 8);
	}
	return out;
}
cv::Rect Processing::getAoiRect()
{
	return data.getAreaOfInterest();
}
void Processing::setAoi(cv::Rect newAoi)
{
	data.setAoi(newAoi);
}
cv::Mat Processing::getExtracted(int back, int exclIdx, int inclIdx, int invsIdx)
{
	cv::Mat img;
	if (back)	//background is corrected binary
	{
		img = binary[0].clone();
		cv::cvtColor(img, img, CV_GRAY2RGB);
	}
	else  //background is original
	{
		img = image.clone();
	}

	for (unsigned i = 0; i < exclusions.size(); ++i)
	{
		//cv::rectangle(img, exclusions[i], cv::Scalar(0, 0, 255), -1, 8, 0);
		cv::Mat roi = img(exclusions[i]);
		cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 255));
		double alpha = 0.2;
		cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
	}

	PointList::iterator it;
	for (it = points.begin(); it != points.end(); ++it) {
		cv::circle(img, *it, 3, cv::Scalar(255, 0, 0), -1, 8, 0);
	}

	for (unsigned i = 0; i < inclusions.size(); ++i)
	{
		for (unsigned j = 0; j < inclusions[i].size(); ++j)
		{
			cv::circle(img, inclusions[i][j], 3, cv::Scalar(0, 255, 0), -1, 8, 0);
		}
	}

	for (unsigned i = 0; i < inversionPoints.size(); ++i)
	{
		cv::line(img, cv::Point(inversionPoints[i], 0), cv::Point(inversionPoints[i], img.rows), cv::Scalar(0, 0, 255), 3, 8, 0);
	}

	if (exclIdx >= 0)
	{
		cv::rectangle(img, exclusions[exclIdx], cv::Scalar(255, 255, 0), 2, 8, 0);
	}
	if (inclIdx >= 0)
	{
		for (unsigned j = 0; j < inclusions[inclIdx].size(); ++j)
		{
			cv::circle(img, inclusions[inclIdx][j], 3, cv::Scalar(255, 255, 0), -1, 8, 0);
		}
	}
	if (invsIdx >= 0)
	{
		cv::line(img, cv::Point(inversionPoints[invsIdx], 0), cv::Point(inversionPoints[invsIdx], img.rows), cv::Scalar(255, 255, 0), 3, 8, 0);
	}
	return img;
}
cv::Mat Processing::getFinal()
{
	cv::Mat img = image.clone();
	PointList::iterator it;
	for (std::vector<PointList>::iterator segIt = segments.begin(); segIt != segments.end(); ++segIt){
		PointList points = *segIt;
		for (it = points.begin(); it != points.end(); ++it) {
			cv::circle(img, *it, 3, cv::Scalar(255, 0, 0), -1, 8, 0);
		}
	}
	return img;
}
cv::Mat Processing::getDebug()
{
	segmentGrid();
	cv::Mat img = getFinal();
	int step = std::round(data.getAreaOfInterest().width * 60.0 / 1440.0);
	for (unsigned i = 0; i < gridPointsX.size(); i += step)
	{
		cv::line(img, cv::Point(gridPointsX[i], 0), cv::Point(gridPointsX[i], img.rows), cv::Scalar(255, 140, 0), 1, 8, 0);
	}
	return img;
}
void Processing::setImage(cv::Mat input)
{
	points.clear();
	cv::Mat imCie = input.clone();
	cv::cvtColor(imCie, imCie, CV_BGR2Lab);
	cv::split(imCie, channels);
	image = input.clone();
	cv::cvtColor(image, image, CV_BGR2RGB);
	if (!data.apply(channels[2]))cerr << "<ERROR>Calibration failed!!!" << endl;
}
void Processing::threshold(int a1, int a2, int b1, int b2)
{
	thresholder.apply(channels[1], binary[1], a1, a2);
	thresholder.apply(channels[2], binary[2], b1, b2);
	binary[0] = binary[1] | binary[2];
	corrected = binary[0].clone();
}
void Processing::extract()
{
	points.clear();
	extr.setParameters(data.getOuterRectangle(), data.getInnerRectangle(), data.getAreaOfInterest());
	applyCorrections();
	extr.apply(corrected, points);
}
bool Processing::addExclusion(cv::Rect rect)
{
	// if rect is out of bounds
	if (rect.br().x < 0 || rect.br().y < 0 || rect.x > corrected.size().width || rect.y > corrected.size().height)
	{
		return false;
	}
	// if rect is over the edge
	if (rect.x < 0)
	{
		rect.width += rect.x;
		rect.x = 0;
	}
	if (rect.y < 0)
	{
		rect.height += rect.y;
		rect.y = 0;
	}
	if (rect.br().x > corrected.size().width)
	{
		rect.width -= (rect.br().x - corrected.size().width);
	}
	if (rect.br().y > corrected.size().height)
	{
		rect.height -= (rect.br().y - corrected.size().height);
	}
	exclusions.push_back(rect);
	cv::rectangle(corrected, rect, cv::Scalar(0, 0, 0), -1, 8, 0);
	return true;
}
void Processing::removeExclusion(int idx)
{
	exclusions.erase(exclusions.begin() + idx);
	corrected = binary[0].clone();
}
void Processing::addInclusion(std::vector<cv::Point> path)
{
	inclusions.push_back(path);
}
void Processing::removeInclusion(int idx)
{
	inclusions.erase(inclusions.begin() + idx);
	corrected = binary[0].clone();
}
void Processing::addInversion(int x)
{
	inversionPoints.push_back(x);
}
void Processing::removeInversion(int idx)
{
	inversionPoints.erase(inversionPoints.begin() + idx);
	corrected = binary[0].clone();
}
void Processing::resetCorrections()
{
	corrected = binary[0].clone();
	exclusions.clear();
	inclusions.clear();
	inversionPoints.clear();
}
void Processing::applyCorrections()
{
	for (unsigned i = 0; i < exclusions.size(); ++i)
	{
		cv::rectangle(corrected, exclusions[i], cv::Scalar(0, 0, 0), -1, 8, 0);
	}
}
void Processing::makeGrid(int timespan)
{
	gridPointsX.clear();
	int totalW = data.getAreaOfInterest().width;
	int count = data.getAreaOfInterest().x;

	int precisionTimer = -1;
	int stepsToFix = 0;
	int errorIsPositive = false;
	double stepDouble = totalW / static_cast<double>(timespan);
	int step = std::round(stepDouble);
	double windowRemainder = step - stepDouble;
	if (windowRemainder != 0.0)
	{
		stepsToFix = std::abs(1 / windowRemainder);
		if (windowRemainder > 0) errorIsPositive = true;
		else errorIsPositive = false;

		if (stepsToFix < timespan)
		{
			precisionTimer = stepsToFix;
		}
	}

	while (gridPointsX.size() < timespan)
	{
		precisionTimer--;
		if (precisionTimer == 0)
		{
			errorIsPositive ? count -= 1: count += 1;
			precisionTimer = stepsToFix;
		}
		count += step;
		gridPointsX.push_back(count);
	}
	gridPointsY.clear();
	int totalH = data.getAreaOfInterest().height;
	step = std::round(totalH / 10.0);
	count = data.getAreaOfInterest().y;
	while (gridPointsY.size() < 10)
	{
		gridPointsY.push_back(count);
		count += step;
	}
}
void Processing::segmentGrid()
{
	gridPointsX.clear();
	for (int i = 0; i < segments.size(); i++)
	{
		for (int j = 0; j < segments[i].size(); j++)
		{
			gridPointsX.push_back(segments[i][j].x);
		}
	}
}
void Processing::postProcess(int resolution)
{
	for (unsigned i = 0; i < inclusions.size(); ++i)
	{
		for (unsigned j = 0; j < inclusions[i].size(); ++j)
		{
			if (inclusions[i][j].y >= 0)
			{
				points.push_back(inclusions[i][j]);
			}
		}
	}
	points.flattenX();
	points.sort();
	postProcessor.apply(points, segments, inversionPoints, data);
}
string Processing::results(int resolution, string datestr, int timespan)
{
	std::vector<float> result;
	converter.apply(segments, result, data, resolution, timespan);

	std::stringstream ss;
	ss.setf(ios_base::fixed, ios_base::floatfield);
	ss.precision(1);

	boost::posix_time::ptime time = boost::posix_time::time_from_string(datestr);
	boost::posix_time::time_iterator timeit(time, boost::posix_time::minutes(resolution));
	++timeit;

	boost::posix_time::time_facet * facet = new boost::posix_time::time_facet("%Y-%m-%d\t%H:%M");
	ss.imbue(std::locale(std::cout.getloc(), facet));

	for (std::vector<float>::iterator it = result.begin(); it != result.end(); ++it) {
		ss << *timeit << "\t" << *it << endl;
		++timeit;
	}
	return ss.str();
}
string Processing::entireProcess(int resolution, string datestr, int timespan, int thresh[4])
{
	threshold(thresh[0], thresh[1], thresh[2], thresh[3]);
	extract();
	postProcess(resolution);
	return results(resolution, datestr, timespan);
}