#ifndef _Function_H_
#define _Function_H_

#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <cmath>
#include <time.h>
#include <iomanip>
#include <map>
#include<sys/stat.h>
#include<unistd.h>
#include "Exception.h"
#include "Configure.h"




double Speed(int l);

int RandNum(int l, int r); //random generate a integer in [l, r]

double RandDouble(double l, double r); //random generate a fraction in [l, r]


std::string itos(const int i);

namespace myself
{
	bool createDirectory(std::string folder);

}

template<class T>
void CreateArray_01(T*& arr, int m)
{
	arr = new T[m + 1];
}

template<class T>
void CreateArray_02(T**& arr, int m, int n)
{
	CreateArray_01(arr, m);
	for (int i = 0; i <= m; i++)
		CreateArray_01(arr[i], n);
}


template<class T>
void ReadData_02(std::string a, T**& arr, int m, int n)
{
	std::fstream ifs(a);
	if (ifs.is_open())
		std::cout << "open success" << std::endl;
	else
		std::cout << "open fail" << std::endl;


	for (int i = 1; i <= m; i++)
		for (int j = 1; j <= n; j++)
			ifs >> arr[i][j];

	ifs.close();
}

template<class T>
void Release_01(T*& a)
{
	if (a == NULL) { return; }
	delete[] a;
	a = NULL;
}


template<class T>
void Release_02(T**& a, const int m)
{
	if (a == NULL) { return; }
	for (int i = 0; i <= m; i++)
	{
		delete[] a[i];
	}
	delete[] a;
	//std::cout << "array has been released !" << std::endl;
	a = NULL;
}


template<class T>
int indexOfMax(T a[], int n)
{// Locate the largest element in a[0:n-1].
	if (n <= 0)
		std::cout << "parameters error !" << std::endl;

	int indexOfMax = 0;
	for (int i = 1; i < n; i++)
		if (a[indexOfMax] < a[i])
			indexOfMax = i;
	return indexOfMax;
}


template <class T>
T select(T a[], int n, int k)
{// Return k'th smallest element in a[0 : n - 1].
	if (k < 1 || k > n)
		std::cout << "parameters error !" << std::endl;

	// move largest element to right end
	int max = indexOfMax(a, n);
	std::swap(a[n - 1], a[max]);
	return select(a, 0, n - 1, k);
}

template <class T>
T select(T a[], int leftEnd, int rightEnd, int k)
{// Return k'th element in a[leftEnd:rightEnd].
	if (leftEnd >= rightEnd)
		return a[leftEnd];
	int leftCursor = leftEnd,        // left-to-right cursor
		rightCursor = rightEnd + 1;  // right-to-left cursor
	T pivot = a[leftEnd];

	// swap elements >= pivot on left side
	// with elements <= pivot on right side
	while (true)
	{
		do
		{// find >= element on left side
			leftCursor++;
		} while (a[leftCursor] < pivot);

		do
		{// find <= element on right side
			rightCursor--;
		} while (a[rightCursor] > pivot);

		if (leftCursor >= rightCursor) break;  // swap pair not found
		std::swap(a[leftCursor], a[rightCursor]);
	}

	if (rightCursor - leftEnd + 1 == k)
		return pivot;

	// place pivot
	a[leftEnd] = a[rightCursor];
	a[rightCursor] = pivot;

	// recursive call on one segment
	if (rightCursor - leftEnd + 1 < k)
		return select(a, rightCursor + 1, rightEnd,
			k - rightCursor + leftEnd - 1);
	else return select(a, leftEnd, rightCursor - 1, k);
}


#endif // !_Function_H_




