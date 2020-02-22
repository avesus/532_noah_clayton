#pragma once

#include <mutex>
#include <shared_mutex>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

#include "line.h"

using namespace std;


enum {
	success,
	cant_open_file,
	join_exception
};


class Play {
	string name;
	unsigned counter;
	int n_chars;
	int n_passed;
	string cur_char;

	mutex m;
	condition_variable cv;

public:

    Play(const string &name): name(name), n_chars(0), n_passed(0), counter(1) {}

	void recite(set<line>::iterator& it);

	void set_n_chars(int n) {
		n_chars = n;
	}

	void player_exit() {
		lock_guard<mutex> l(m);
		if (n_passed == --n_chars && n_chars > 0) {
			// nobody could read this line, need to skip it
			cerr << endl << "****** line " << counter << " skipped ******";
			counter++;
			n_passed = 0;
			cv.notify_all();
		}
	}

};

