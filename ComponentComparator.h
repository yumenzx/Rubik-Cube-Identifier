#pragma once

#include "Component.h"

struct ComponentComparator_X {
	bool operator ()(const Component& c1, const Component& c2) {
		if (c1.centerX == c2.centerX)
			return c1.centerY < c2.centerY;
		return c1.centerX < c2.centerX;
	}
};

struct ComponentComparator_Y {
	bool operator ()(const Component& c1, const Component& c2) {
		if (c1.centerY == c2.centerY)
			return c1.centerX < c2.centerX;
		return c1.centerY < c2.centerY;
	}
};