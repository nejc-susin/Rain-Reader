/*
* SimpleHistogram.cpp
*
*  Created on: Feb 4, 2009
*      Author: Gasper Derganc
*	Updated in 2015
*		By: Nejc Susin
*/

#include "SimpleHistogram.h"


SimpleHistogram::SimpleHistogram(){
	data = std::vector<int>(0, 0);
	count = 0;
}

SimpleHistogram::SimpleHistogram(int initialSize){
	data = std::vector<int>(initialSize, 0);
	count = 0;
}

void SimpleHistogram::add(int index){
	data[index]++;
	count++;
}

void SimpleHistogram::take(int index){
	if (data[index] > 0)
		data[index]--;
	if (count > 0)
		count--;
}

void SimpleHistogram::clear(){
	data.clear();
}

int SimpleHistogram::getNumberOfEntries(){
	return count;
}

int SimpleHistogram::getSize(){
	return data.size();
}

int SimpleHistogram::getLastCount(){
	return last_count;
}

int SimpleHistogram::at(int index){
	return data[index];
}


int SimpleHistogram::average(int ulimit, int dlimit){
	long sum = 0;
	last_count = 0;
	if (getNumberOfEntries() == 0)return -1;
	for (int i = ulimit; i < dlimit; i++){
		if (i >= 0 && i < getSize() && at(i) != 0){
			sum += i*at(i);
			last_count += at(i);
		}
	}
	if (last_count == 0) return -1;
	return static_cast<int>(float(sum) / float(last_count));
}

int SimpleHistogram::average(){
	long sum = 0;
	if (getNumberOfEntries() == 0)return -1;
	for (int i = 0; i < getSize(); i++){
		if (at(i) != 0){
			sum += i*at(i);
		}
	}
	last_count = count;
	return static_cast<int>(float(sum) / float(count));
}

int SimpleHistogram::indexOfMax(){
	last_count = at(0);
	int maxIndex = -1;
	if (getNumberOfEntries() == 0) return -1;
	for (int i = 0; i<getSize(); i++){
		if (at(i) < last_count){
			last_count = at(i);
			maxIndex = i;
		}
	}
	return maxIndex;
}

void SimpleHistogram::resize(int size){
	count = 0;
	last_count = 0;
	data.resize(size);
	data.reserve(size);
	data.clear();
}




