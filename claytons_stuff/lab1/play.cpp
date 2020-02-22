
#include <algorithm>
#include <thread>
#include <unordered_map>

#include "play.h"

using std::endl;

void Play::recite(set<line>::iterator& it) {
	unique_lock<mutex> l(m);
	while (counter < it->linen) {
		n_passed++;
		if (n_passed == n_chars) {
			// nobody could read this line, need to skip it
			cerr << endl << "****** line " << counter << " skipped ******";
			counter++;
			n_passed = 0;
			cv.notify_all();
			continue;
		}
		cv.wait(l);
	}
	if (counter > it->linen) {
		cerr << endl << "****** line " << it->linen << " said by "
			<< it->character << " skipped ******";

		cv.notify_all();
		return;
	}
	if (cur_char != it->character) {
		if (cur_char != "") {
			cout << endl << endl;
		}
		cur_char = it->character;
		cout << it->character << '.';
	}
	cout << endl << it->msg;
	counter++;
	n_passed = 0;
	cv.notify_all();
}

