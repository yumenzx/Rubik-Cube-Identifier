#pragma once

#include <opencv2/core/core.hpp>

class Component
{
public:
    int area;
    int centerX, centerY;
    cv::Vec3b color;

    Component(int area, int centerX, int centerY, cv::Vec3b color);
    Component(const Component& other);

    cv::Point centerMass() const;

    bool operator<(const Component& other) const;
    Component& operator=(const Component& other);
};

