#pragma once
#include <iostream>
using namespace std;
class TestObj
{
private:
	string GetNewGUID();
public:
	int Id;
	const char* Name;
	const char * Value;
	TestObj();
};

