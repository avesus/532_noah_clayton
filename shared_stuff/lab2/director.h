
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <cstring>

#include "sync_que.h"
#include "helpers/opt.h"
#include "player.h"

using namespace std;



class Director {
    
    ifstream i;
    vector<string> names;
    vector<istringstream> configs;
    vector<int> n_players;
    shared_ptr<Play> p;
    vector<shared_ptr<Player>> players;

    mutex m;
    condition_variable cv;

    sync_que frag_que;

public:

    Director(const string &name, uint32_t min_players=0) : i(name) {
        if (!i.is_open()) {
            throw std::runtime_error("couldn't open file for reading");
        }

        string buf;
        int max = 0, prev = 0;
        int scene = 0;
        while (getline(i, buf)) {
            if (strncmp(buf.c_str(), "[scene] ", strlen("[scene] ")) == 0) {
                names.push_back(buf.substr(strlen("[scene] ")));
                scene = 1;
            }
            else {
                if (!scene) {
                    names.push_back("");
                }
                ifstream config(buf);
                if (config.is_open()) {
                    int n_chars = 0;
                    string buf2;
                    while (getline(config, buf2)) {
                        n_chars++;
                    }
                    if (!scene) {
                        // there is a config file before this
                        max = MAX(max, prev + n_chars);
                        prev = n_chars;
                    }
                    configs.push_back(istringstream(buf));
                    n_players.push_back(n_chars);
                }
                scene = 0;
            }
        }

        p = make_shared<Play>(names);
        for (int j = 0; j < MAX(max, min_players); j++) {
            players.push_back(make_shared<Player>(*p));
            players[j]->work(this->frag_que);
        }
    }


    int cue() {
        unique_lock<mutex> lock(m);
        for (size_t frag = 0; frag < configs.size(); frag++) {
            cv.wait(lock, [&] { return !(p->get_on_stage() + frag_que.size()); });
            p_info p;
            p.frag_num = frag;
            this->p->set_on_stage(n_players[frag]);
            string buf;
            while (getline(this->configs[frag], buf)) {
                if (istringstream(buf) >> p.name >> p.file) {
                    frag_que.push(p);
                }
            }
        }
    }
};


