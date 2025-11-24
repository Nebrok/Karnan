#pragma once
#include <iostream>

//#include <windows.h>
#include <functional>


namespace Karnan
{
	/*
	void PrintMemoryStatus()
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);

		GlobalMemoryStatusEx(&statex);
		std::cout << "There is " << statex.ullAvailPhys / (1024 * 1024) << " MB of physical memory availiable." << '\n';
		std::cout << "There is " << statex.ullAvailPageFile / (1024 * 1024 * 1024) << " GB of page file memory availiable." << '\n';
	}
	*/

	
	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void HashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(HashCombine(seed, rest), ...);
	};
	
}