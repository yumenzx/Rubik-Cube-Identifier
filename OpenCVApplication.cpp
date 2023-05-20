// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include <random>
#include <stack>
#include <fstream>
#include <opencv2/core.hpp>
#include "processingFunctions.h"



inline void app()
{
	Mat img = imread("./Images/CUBE_MAP.png", IMREAD_COLOR);
	
	char fName[3][MAX_PATH];

	std::thread t[3];
	THREAD_ARG arg[3];
	
	int k;
	for (k = 0; k < 3; k++) {
		arg[k].img = &img;
		arg[k].fName = fName[k];
		arg[k].id = k + 1;

		if (openFileDlg(fName[k]))
			t[k] = std::thread(beginProcess, std::ref(arg[k]));
		else
			goto _user_exit;
	}

	for (k = 0; k < 3; k++)
		t[k].join();

	imshow("CUBE MAP", img);
	waitKey(0);
	return;

_user_exit:
	for (auto l = 0; l < k; l++)
		t[l].detach();
}

int main()
{
	app();

	return 0;

	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);

	return 0;
}