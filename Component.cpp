#include "stdafx.h"
#include "Component.h"

Component::Component(int area, int centerX, int centerY, cv::Vec3b color) : area(area), centerX(centerX), centerY(centerY), color(color) {
}

Component::Component(const Component& other) : area(other.area), centerX(other.centerX), centerY(other.centerY), color(other.color) {
}


cv::Point Component::centerMass() const{
	return cv::Point(centerX, centerY);
}

bool Component::operator<(const Component& other) const {
    return area < other.area;
}

Component& Component::operator=(const Component& other) {
    if (this != &other) {
        area = other.area;
        centerX = other.centerX;
        centerY = other.centerY;
        color = other.color;
    }
    return *this;
}