#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <list>
#include <queue>
#include "stdafx.h"
#include "Component.h"
#include "ComponentComparator.h"


struct THREAD_ARG {
	cv::Mat* img;
	char* fName;
	int id;
};

void coloreaza(cv::Mat sourceGray, cv::Mat sourceColored, cv::Mat dest, cv::Point p, cv::Vec3b color);
void processComponents(std::list<Component>& components);

std::list<Component> computeComponents(const cv::Mat& source, const cv::Mat& sourceColored);

cv::Mat convertToBinary(const cv::Mat& source);

void beginProcess(THREAD_ARG& arg);