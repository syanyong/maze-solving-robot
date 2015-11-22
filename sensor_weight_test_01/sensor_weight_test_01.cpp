// sensor_weight_test_01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

using namespace std;
unsigned int y[8] = {0,0,11,10,0,0,0,0};		//  |<|<|<|^|^|>|>|>|
int left0,right0,center0;
int center1;
int _tmain(int argc, _TCHAR* argv[])
{
	// Different Rule 0
	left0 = y[2] - y[3];			
	right0 = y[5] - y[4];			
	center0 = y[4] - y[3];
	center1 = right0 - left0 + center0*2;
	cout << "LR0" << " | " << left0 << " | " << right0 <<" | " << endl;
	cout << "CE" << " | " <<  center1 << " | " << endl;
	/*
	// Different Rule 1
	left0 = y[2]*2 - y[3];			//  |<|<|^|^|>|>|>|>| -> |^|>|>|
	center0 = y[4] - y[3];			//  |<|<|<|<|^|^|>|>| -> |<|<|^|
	right0 = y[5]*2 - y[4];			//  |<|<|<|^|^|>|>|>| -> |<|^|>|
	cout << " | " << left0 << " | " << center0 << " | " << right0 << " | " << endl;
	left1 = center0 - left0;		//  |^|^|>|	-> |^|>|
	right1 = center0 - right0;		//  |<|^|^|	-> |<|^|
	cout << " | " << left1 << " | " << right1 <<" | " << endl;
	center2 = left1 - right1;
	cout << " | " << center2 << " | " << endl;
	*/

	system("pause");
	return 0;
}

