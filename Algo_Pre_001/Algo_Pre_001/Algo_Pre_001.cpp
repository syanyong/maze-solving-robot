// Algo_Pre_001.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>

using namespace std;
int iW = 0;
int iS = 0;
unsigned char Walked[100];
unsigned char WalkBack[100];
unsigned char Inp;
char getData(unsigned char);
void showData(unsigned char*);
void processData(void);
void debuf(unsigned char,unsigned char,unsigned char*);

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "W A L K I N G\n\r" << endl;
	do{
		cin >> Inp;
	}while(!getData(Inp));
	cout << "------>Finished<------" << endl;
	cout << iS << " Step"<< endl;
	showData(Walked);
	processData();
	showData(WalkBack);
	system("PAUSE");
	return 0;
}

char getData(unsigned char data){
	switch (data){
		case '1' : cout << "Left(1)" << endl;
			Walked[iW++] = 1;
			break;
		case '2' : cout << "Right(2)" << endl;
			Walked[iW++] = 2;
			break;
		case '3' : cout << "Left(1)" << endl;
			Walked[iW++] = 1;
			break;
		case '4' : cout << "Follow(3)" << endl;
			Walked[iW++] = 3;
			break;
		case '5' : cout << "Left(1)" << endl;
			Walked[iW++] = 1;
			break;
		case '6' : cout << "Left(1)" << endl;
			Walked[iW++] = 1;
			break;
		case '7' : cout << "TurnOver(4)" << endl;
			Walked[iW++] = 4;
			break;
		case '8' : cout << "BOX" << endl;
			iS--;
			break;
	}
	iS++;
	if(data == '8'){
		return 1;
	}else{
		return 0;
	}
	
}
void showData(unsigned char* data){
		cout << "DATA "<<" = " ;
		for(int i = 0; *(data+i) != '\0';i++){
			cout << *(data+i)+0 <<">";
		}
		cout << endl;	
}

void debuf(unsigned char dIn,unsigned char m,unsigned char* data){
	int i = 0;
	int count = 0;
	unsigned char temp;
	for(count = 0;*(data+count) != '\0';count++);
	*(data+m-1) = dIn;
	for(i = m; i<=count ;i++){
		temp = *(data+i);
		*(data+i) = *(data+i+2);
		*(data+i+2) = temp;
	}
}

void processData(void){
		int i = 0;
		/*// For First Case
		if(Walked[iS-1] == 3){
			// Follow To TurnOver
			WalkBack[0] = 4;
		}else if(Walked[iS-1] == 2){
			// TurnRight To TurnRight
			WalkBack[0] = 2;
		}else if(Walked[iS-1] == 1){
			// TurnLeft To TurnLeft
			WalkBack[0] = 1;
		}*/
		// Invert
		for(i = 0;i < iS;i++){
			switch (Walked[(iS-1) - i]){
				case 1 : WalkBack[i] = 2;		// TurnLeft To TurnRight.
					break;	
				case 2 : WalkBack[i] = 1;		// TurnRight To TurnLeft
					break;
				case 3 : WalkBack[i] = 3;		// Forward To Forward
					break;
				case 4 : WalkBack[i] = 4;		// TurnOver To TurnOver
					break;
			}
		
		}
		showData(WalkBack);
		// Shortest Part
		for(i = 1;WalkBack[i] != '\0';i++){
			if(WalkBack[i] == 4 ){
				// Case 2->4->3
				if((WalkBack[i-1] == 2)&&(WalkBack[i+1] == 3)){
					debuf(1,i,WalkBack);	
				}
				// Case 3->4->2
				if((WalkBack[i-1] == 3)&&(WalkBack[i+1] == 2)){
					debuf(1,i,WalkBack);	
				}
				// Case 2->4->2
				if((WalkBack[i-1] == 2)&&(WalkBack[i+1] == 2)){
					debuf(3,i,WalkBack);	
				}
			}
		}

}
