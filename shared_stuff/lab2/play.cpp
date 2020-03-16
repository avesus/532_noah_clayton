
#include <algorithm>
#include <thread>
#include <unordered_map>

#include "play.h"

using std::endl;


void Play::recite(set<line>::iterator& it, int32_t expec_fragments_lines) {
    unique_lock<mutex> l(m);
    while ((line_counter < it->linen &&
                this->scene_fragment_counter == expec_fragments_lines) ||
	   (this->scene_fragment_counter < expec_fragments_lines)
          ){

        n_passed++;

        if (n_passed == on_stage) {
            // nobody could read this line, need to skip it
            cerr << endl << "****** line " << line_counter << " skipped ******";
            line_counter++;
            n_passed = 0;
            cv.notify_all();
            continue;
        }
        cv.wait(l);
    }
    if (line_counter != it->linen ||
            this->scene_fragment_counter > expec_fragments_lines) {
        cerr << endl << "****** line " << it->linen << " said by "
            << it->character << " skipped fragment ******";
        this->it++;
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
    cout << endl << it->linen << ": "<< it->msg;
    line_counter++;
    n_passed = 0;
    cv.notify_all();
}

