#pragma once
#include <windows.h>
#include <vector>
#include <string>
using namespace std;

typedef struct tagTempData {
	DWORD   dwTemperature;
	DWORD   dwTimeIndex;
}TempData;

template <class T>
void ClearVector(vector<T> & v) {
	typedef vector<T>::iterator v_it;
	v_it it;
	for (it = v.begin(); it != v.end(); it++) {
		delete *it;
	}
	v.clear();
}