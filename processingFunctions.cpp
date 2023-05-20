#include "stdafx.h"
#include "processingFunctions.h"
#include <opencv2/highgui.hpp>


cv::Mat convertToBinary(const cv::Mat& source)
{
	cv::Mat inputImage = source;

	cv::Mat binaryImage;
	// Apply adaptive thresholding with a block size of 11 and a constant offset of 2
	cv::adaptiveThreshold(inputImage, binaryImage, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 2);

	// Define the structuring element to use for the close operation
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	// Perform the close then open operation to filter
	cv::Mat closedImage, openImage;
	cv::morphologyEx(binaryImage, closedImage, cv::MORPH_CLOSE, element);
	cv::morphologyEx(closedImage, openImage, cv::MORPH_OPEN, element);


	cv::Mat edgesImage;
	// Edge detection
	cv::Canny(inputImage, edgesImage, 100, 200);


	// Invert the filtred image
	cv::bitwise_not(openImage, openImage);

	cv::Mat overlapImage;
	// Overlap binary filtred image with the edges image
	cv::bitwise_or(openImage, edgesImage, overlapImage);

	return overlapImage;
}

std::list<Component> computeComponents(const cv::Mat& source, const cv::Mat& sourceColored)
{
	int di[8] = { -1,-1,-1, 0, 0, 1,1,1 };
	int dj[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	cv::Mat image = source;
	const int32_t height = image.rows;
	const int32_t width = image.cols;

	cv::Mat labels = cv::Mat::zeros(height, width, CV_8UC1);
	cv::Mat processed(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

	std::list<Component> components;
	uchar label = 0;
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {

			uchar color = image.at<uchar>(i, j);
			if ((color == 0) && (labels.at<uchar>(i, j) == 0)) {
				label++;

				int area = 1;
				int centerX = 0, centerY = 0;
				uint R = 0, G = 0, B = 0;

				std::queue<cv::Point> Q;
				labels.at<uchar>(i, j) = label;

				Q.push(cv::Point(i, j));
				while (!Q.empty()) {
					cv::Point q = Q.front();
					Q.pop();

					for (int k = 0; k < 8; k++) {
						auto dx = q.x + di[k];
						auto dy = q.y + dj[k];
						if (0 <= dx && dx < height && 0 <= dy && dy < width)
							if ((image.at<uchar>(dx, dy) == 0) && (labels.at<uchar>(dx, dy) == 0)) {
								labels.at<uchar>(dx, dy) = label;
								Q.push(cv::Point(dx, dy));

								cv::Vec3b pixel = sourceColored.at<cv::Vec3b>(dx, dy);
								R += pixel[2];
								G += pixel[1];
								B += pixel[0];

								area++;
								centerX += dx;	centerY += dy;
							}
					}
				}
				if (area < 100)
					continue;

				cv::Vec3b colorAvg((uchar)(B / area), (uchar)(G / area), (uchar)(R / area));

				Component c(area, centerX / area, centerY / area, colorAvg);
				components.push_back(c);

			}
		}
	}

	components.sort();
	return components;
}


void processComponents(std::list<Component>& components)
{
	components.reverse();
	components.pop_front();

	
	//std::list<Component> l(components.begin(), std::next(components.begin(), 18));
	//printf("%d\n",components.size());
	/*std::cout << l.size() << '\n';
	components.assign(l.begin(), l.end());
	std::cout << components.size() << '\n';
	*/

	std::list<Component> l;
	int i = 18;
	for (auto c : components) {
		if (i == 0)
			break; // de refacut cu std::next(begin(),18)
		l.push_front(c);
		--i;
	}

	//std::cout << components.size() << '\n';
	//std::cout << l.size() << '\n';
	components.assign(l.begin(), l.end());
	///std::cout << components.size() << '\n';

	// dupa mediana
	/*int median;
	auto it = std::next(components.begin(), components.size() / 2);
	median = it->getArie();
	if (components.size() % 2 == 0)
		median = (median + (--it)->getArie()) / 2;

	std::cout << median << '\n';
	for (auto it = components.begin(); it != components.end();)
	{
		auto& c = *it;
		auto area = c.getArie();
		if (area > 100 && median - 500 < area && area < median + 500) {
			++it;
		}
		else
			it = components.erase(it);
	}
	*/
}


static void fillImage(cv::Mat& src, cv::Mat& dest, cv::Point start, cv::Vec3b color)
{
	int di[8] = { -1,-1,-1, 0, 0, 1,1,1 };
	int dj[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	cv::Mat image = src;
	const int32_t height = image.rows;
	const int32_t width = image.cols;

	cv::Mat labels = cv::Mat::zeros(height, width, CV_8UC1);

	std::queue<cv::Point> Q;
	Q.push(start);
	while (!Q.empty()) {
		cv::Point q = Q.front();
		Q.pop();

		dest.at<cv::Vec3b>(q.x, q.y) = color;
		for (int k = 0; k < 8; k++) {
			auto xx = q.x + di[k];
			auto yy = q.y + dj[k];
			if (0 <= xx && xx < height && 0 <= yy && yy < width)
				if ((image.at<uchar>(q.x + di[k], q.y + dj[k]) == 255) && (labels.at<uchar>(q.x + di[k], q.y + dj[k]) == 0)) {
					Q.push(cv::Point(q.x + di[k], q.y + dj[k]));
					labels.at<uchar>(q.x + di[k], q.y + dj[k]) = 1;
				}
		}
	}
}

void coloreaza(cv::Mat sourceGray, cv::Mat sourceColored, cv::Mat dest, cv::Point p, cv::Vec3b color)
{
	int di[8] = { -1,-1,-1, 0, 0, 1,1,1 };
	int dj[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	cv::Mat image = sourceGray;
	const int32_t height = image.rows;
	const int32_t width = image.cols;

	cv::Mat labels = cv::Mat::zeros(height, width, CV_8UC1);

	std::queue<cv::Point> Q;
	Q.push(p);
	while (!Q.empty()) {
		cv::Point q = Q.front();
		Q.pop();
		dest.at<cv::Vec3b>(q.x, q.y) = color;
		for (int k = 0; k < 8; k++) {
			auto xx = q.x + di[k];
			auto yy = q.y + dj[k];
			if (0 <= xx && xx < height && 0 <= yy && yy < width)
				if ((image.at<uchar>(q.x + di[k], q.y + dj[k]) == 0) && (labels.at<uchar>(q.x + di[k], q.y + dj[k]) == 0)) {
					Q.push({ q.x + di[k], q.y + dj[k] });
					labels.at<uchar>(q.x + di[k], q.y + dj[k]) = 1;
				}
		}
	}
}

static std::string printColor(cv::Vec3b color)
{
	cv::Mat3f matRGB(1, 1, CV_32FC3);
	cv::Mat3f matHSV(1, 1, CV_32FC3);

	// rgb -> hsv
	matRGB(0, 0) = cv::Vec3f(color[2] / 255.0f, color[1] / 255.0f, color[0] / 255.0f);
	cv::cvtColor(matRGB, matHSV, cv::COLOR_RGB2HSV);


	auto hue = matHSV(0, 0)[0];
	auto saturation = matHSV(0, 0)[1];
	auto value = matHSV(0, 0)[2];

	printf("hue=%f -> ", hue);

	// de aici se face maparea
	if (hue >= 0 && hue < 30)
		return "red";

	return "UNKOWN";

}

static void mapComponentsColor(std::list<Component>& components, cv::Vec3b colors[6][3])
{
	components.sort(ComponentComparator_X());

	auto it = components.begin();
	for (int k = 0; k < 6; k++) {
		std::list<Component> extractedComponents(it, std::next(it, 3));
		it = std::next(it, 3); // move ptr to next 3 elemets

		extractedComponents.sort(ComponentComparator_Y());
		if (extractedComponents.size() != 3) {
			puts("err");
			return;
		}

		int i = 0;
		for (auto c : extractedComponents) {
			colors[k][i++] = c.color;
		}
	}

}

static void getStartPoints(int threadId, cv::Point p[6][3])
{
	if (threadId == 1) {
		int startX = 30;
		int startY = 230;
		int diff = 65;

		for (int i = 0; i < 6; i++)
			for (int j = 0; j < 3; j++)
				p[i][j] = cv::Point(startX + i * diff, startY + j * diff);

		return;
	}

	if (threadId == 2) {
		int startX = 360;
		int startY = 420;
		int diff = 65;

		for (int i = 0; i < 6; i++)
			for (int j = 0; j < 3; j++)
				p[i][j] = cv::Point(startX - j * diff, startY + i * diff);

		return;
	}

	if (threadId == 3) {
		int startX = 220;
		int startY = 20;
		int diff = 65;

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				p[i][j] = cv::Point(startX + i * diff, startY + j * diff);

		startX = 550; startY = 220;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				p[i + 3][j] = cv::Point(startX - j * diff, startY + i * diff);

		return;
	}
}

void beginProcess(THREAD_ARG& arg)
{
	char* fname = arg.fName;
	cv::Mat inputImage = cv::imread(fname, cv::IMREAD_GRAYSCALE);
	cv::Mat inputImageColored = cv::imread(fname, cv::IMREAD_COLOR);

	cv::Mat binaryImage = convertToBinary(inputImage);
	//cv::imshow("procxc", binaryImage); // asta il poti comenta ca sa nu iti arate ordinea sortarii
	//cv::waitKey();
	std::list<Component> components = computeComponents(binaryImage, inputImageColored);

	processComponents(components);

	cv::Vec3b colors[6][3];
	mapComponentsColor(components, colors);


	cv::Mat grayImage;
	cv::cvtColor(*arg.img, grayImage, cv::COLOR_BGR2GRAY);

	cv::Point p[6][3];
	getStartPoints(arg.id, p);
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 3; j++)
			fillImage(grayImage, *arg.img, p[i][j], colors[i][j]);


	//return;


	components.sort(ComponentComparator_X());

	cv::Mat processed(binaryImage.rows, binaryImage.cols, CV_8UC3, cv::Scalar(0, 0, 0));
	int i = 100;
	for (auto c : components) {
		if (i == 0)
			break;
		cv::Vec3b color = c.color;
		coloreaza(binaryImage, inputImageColored, processed, c.centerMass(), color);
		i--;
	}

	//puts("gata");
	//cv::imshow("procc", processed); // asta il poti comenta ca sa nu iti arate ordinea sortarii
	//cv::waitKey();
}