#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <Windows.h>
#include <algorithm>


namespace globals {
	// yall know its scanning size..
	inline float ScanningSize = 200;
	//our smoothing
	inline float Smoothing = 0.8;

	inline HWND hWnd = nullptr;


}




