/*
* Converter.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "Converter.h"

Converter::Converter() {
}

Converter::~Converter() {
}

void Converter::apply(std::vector<PointList> segments, std::vector<float> &result, Data data, int resolution, int timespan){

	int width = data.getAreaOfInterest().width;
	int height = data.getAreaOfInterest().height;
	float heightModifier = 10.0 / static_cast<float>(height);
	

	PointList cummulative;
	cummulative.push_back(cv::Point(segments[0].front().x, 0));
	for (int i = 0; i < segments.size(); i++){
		for (int j = 1; j < segments[i].size(); j++){
			cv::Point point = segments[i][j];
			point.y = segments[i][j - 1].y - segments[i][j].y + cummulative.back().y;
			cummulative.push_back(point);
		}
	}
	int timespanMinutes = timespan * 60;

	if (resolution == timespanMinutes){
		result.push_back(static_cast<float>(cummulative.back().y * heightModifier));
		return;
	}

	// number of steps
	int steps = static_cast<int>(timespanMinutes / static_cast<double>(resolution));
	
	// width of one step - number of measurements that will be treated as one
	// we round the number and make up for precision if the error accumulates to more than 1 step
	int precisionTimer = -1;
	int stepsToFix = 0;
	int errorIsPositive = false;
	double windowDouble = width / static_cast<double>(steps);
	int window = std::round(windowDouble);
	double windowRemainder = window - windowDouble;
	if (windowRemainder != 0.0)
	{
		stepsToFix = std::abs(1 / windowRemainder);
		if (windowRemainder > 0) errorIsPositive = true;
		else errorIsPositive = false;
		
		if (stepsToFix < steps)
		{
			precisionTimer = stepsToFix;
		}
	}

	int previous = 0;
	int current = 0;
	int diff = 0;

	for (int i = 1; i <= steps; i++){
		precisionTimer--;
		previous = current;
		current += window;
		if (precisionTimer == 0)
		{
			errorIsPositive ? current -= 1 : current += 1;
			precisionTimer = stepsToFix;
		}
		if (current >= cummulative.size())
		{
			result.push_back(0.0f);
		}
		else
		{
			diff = cummulative[current].y - cummulative[previous].y;
			result.push_back(static_cast<float>(diff * heightModifier));
		}
	}

}