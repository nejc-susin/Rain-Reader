/*
* main.cpp
*
*	Created in 2015
*		By: Nejc Susin
*/

#include <QtWidgets/QApplication>

#include "raingui.h"


int main(int argc, char *argv[]){
	QApplication rain(argc, argv);
	rainGui w;
	w.show();
	rain.exec();
}
