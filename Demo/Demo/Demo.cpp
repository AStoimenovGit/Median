// Demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "Median.h"
#include "Map.h"
#include "AVLTree.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class Compare>
T TestMedian(Median<T, Compare>& median)
{
	median.Insert(3);
	median.Insert(1);
	median.Insert(2);
	median.Insert(2);
	median.Insert(7);
	median.Insert(9);
	median.Insert(4);
	median.Insert(6);

	T	value;
	median.GetMedian(value);
	return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	float	median;

	AVLTree<decltype(median)> tree;
	median = ::TestMedian(tree);

	Map<decltype(median)>	mapMedian;
	median = ::TestMedian(mapMedian);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////