/*
* RegionGrowingThresholder.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "RegionGrowingThresholder.h"
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stack>
#include <algorithm>

RegionGrowingThresholder::RegionGrowingThresholder() {

}

void RegionGrowingThresholder::apply(cv::Mat in, cv::Mat& out, int upperThreshold, int lowerThreshold) {
	cv::Mat visited(in.size().height, in.size().width, CV_8UC1, cv::Scalar(0, 0, 0));
	cv::Mat result(in.size().height, in.size().width, CV_8UC1, cv::Scalar(0, 0, 0));

	visited = cv::Scalar(0, 0, 0);

	bool direction = upperThreshold > lowerThreshold;
	float sum = 0;
	float number = 0;
	float mean = 0;
	float stdev = 0;
	cv::Point seed, tmp, p;
	std::stack<cv::Point> queue;
	int val;

	for (p.y = 0; p.y < in.size().height; p.y++) {
		for (p.x = 0; p.x < in.size().width; p.x++) {
			/*std::cout << p << ": " << in.at<uchar>(p) << std::endl;
			continue;*/
			// new seed found
			if (!visited.at<uchar>(p) && (direction ? ((int)in.at<uchar>(p) >= upperThreshold) : ((int)in.at<uchar>(p) <= upperThreshold))) {
				queue.push(p);
				number = 1;
				sum = in.at<uchar>(p);
				mean = 0;
				stdev = 0;

				//				std::cout<<">>>>>>>growing region at:"<<p<<std::endl;
				//				std::cout<<">>>>threshold:"<<uint(upperThreshold)<<endl;

				while (!queue.empty()) {
					seed = queue.top();
					queue.pop();
					//cout << "seed:"<<seed<< " "<<uint(in.at(seed))<<endl;
					result.at<uchar>(seed) = 255;



					//cv::floodFill(result, seed, 255, 0, upperThreshold, lowerThreshold, 4);




					mean = sum / number;
					// calculate std deviation of neighborhood
					stdev = 0;
					int n = 0;
					for (tmp.x = seed.x - 1; tmp.x <= seed.x + 1; tmp.x++) {
						for (tmp.y = seed.y - 1; tmp.y <= seed.y + 1; tmp.y++) {
							if (tmp.x >= 0 && tmp.x < in.size().width && tmp.y >= 0 && tmp.y < in.size().height) {
								val = in.at<uchar>(tmp);
								stdev += std::pow(val - mean, 2);
								n++;
							}
						}
					}
					stdev /= n;
					stdev = std::sqrt(stdev);
					//cout << "mean:"<<mean<<"\tstdev:"<<stdev<<endl;
					// grow new seeds
					for (tmp.x = seed.x - 1; tmp.x <= seed.x + 1; tmp.x++) {
						for (tmp.y = seed.y - 1; tmp.y <= seed.y + 1; tmp.y++) {
							if (tmp.x >= 0 && tmp.x < in.size().width && tmp.y >= 0 && tmp.y < in.size().height) {
								val = in.at<uchar>(tmp);
								if (!visited.at<uchar>(tmp)) {
									//cout << "threshold: "<< (direction ? uint(lti::max(lowerThreshold, ubyte( mean - stdev))) : uint(lti::min(lowerThreshold, ubyte(mean + stdev))))<<endl;
									//cout<<"checking "<<tmp<<" "<<uint(in.at(tmp))<<" ...";

									//if ((direction ? val >= std::max(lowerThreshold, int(mean - stdev)) : val <= std::min(lowerThreshold, int(mean + stdev)))) {
									//if ((direction ? val >= ( mean - stdev) : val <= (mean + stdev))) {
									if (direction ? val >= lowerThreshold : val <= lowerThreshold){
										queue.push(tmp);
										number += 1;
										sum += in.at<uchar>(tmp);
										//cout << "OK" <<endl;
									}//else cout << endl;
									visited.at<uchar>(tmp) = 255;
								}
							}
						}
					}
				}
			}
		}
	}
	out = result;
}
