#pragma once
#include <string>

using namespace std;

struct line {
	unsigned linen;
	string character;
	string msg;

	bool operator<(const line& l) const {
		return linen < l.linen;
	}
};