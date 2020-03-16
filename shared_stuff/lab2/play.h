#pragma once

#include <mutex>
#include <shared_mutex>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <condition_variable>
#include <fstream>
#include <set>

#include "line.h"

using namespace std;


enum {
    success,
    cant_open_file,
    join_exception,
    not_ready_to_enter,
    exit_failure
};


class Play {
    const vector<string> &names;
    vector<string>::const_iterator it;
    uint32_t line_counter;
    int32_t scene_fragment_counter;
    int32_t on_stage;
    int32_t n_passed;
    string cur_char;

    mutex m;
    condition_variable cv;

public:

    Play(const vector<string> &names): names(names), it(names.begin()),
            line_counter(1), scene_fragment_counter(0), on_stage(0),
            n_passed(0) {}


    void recite(set<line>::iterator& it, int32_t expec_fragment_lines);

    void set_on_stage(int32_t n) {
        on_stage = n;
    }
    void reset_counter(){
      this->line_counter = 1;
    }
    int32_t get_on_stage() {
        return on_stage;
    }

    int enter(int32_t frag) {
        unique_lock<mutex> l(m);
        if (frag < scene_fragment_counter) {
            return not_ready_to_enter;
        }
        while (frag > scene_fragment_counter) {
            cv.wait(l);
        }
        return success;
    }

    int player_exit() {
        lock_guard<mutex> l(m);
        if (n_passed == --on_stage && on_stage > 0) {
            // nobody could read this line, need to skip it
            cerr << endl << "****** line " << line_counter << " skipped ******";
            line_counter++;
            n_passed = 0;
            cv.notify_all();
        }
        if (on_stage < 0) {
            // ahh!
            on_stage++;
            return exit_failure;
        }
        else if (on_stage == 0) {
            scene_fragment_counter++;
            if (it != names.end()) {
                if (*it != "") {
                    cout << *it << endl;
                }
                it++;
                cv.notify_all();
            }
        }
        return success;
    }

};

