#include "MyLibrary.h"

void myFunction(int a, int* b)
{
	a = 10;
	*b = 10;
}

void myUtilityFunc()
{
	int a = 27814678;
	int b = 12341;

	myFunction(a, &b);
	//a = 27814678
	//b = 10
}