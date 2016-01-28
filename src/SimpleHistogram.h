/*
 * SimpleHistogram.h
 *
 *  Created on: Feb 4, 2009
 *      Author: Gasper Derganc
 *	Updated in 2015
 *		By: Nejc Susin
 */

#ifndef _SIMPLE_HISTOGRAM_H_
#define _SIMPLE_HISTOGRAM_H_

#include <vector>
#include <iostream>
using namespace std;

class SimpleHistogram{

	public:
		SimpleHistogram();
		SimpleHistogram(int initialSize);
		void add(int index);
		void take(int index);
		void clear();
		void resize(int size);
		int getNumberOfEntries();
		int getSize();
		int getLastCount();
		int at(int index);
		int average();
		int average(int dlimit, int ulimit);
		int indexOfMax();

	private:
		int count;
		int last_count;
		std::vector<int> data;

};

#endif

