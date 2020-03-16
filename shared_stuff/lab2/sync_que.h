
#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <memory>
#include <condition_variable>
#include <mutex>

#include "helpers/opt.h"



using namespace std;

struct p_info {
    string name;
    size_t frag_num;
    string file;
};


class sync_que {
    mutex m;
    condition_variable cv;
    queue<p_info> que;
    
public:
    void push(p_info p);
    p_info pop();

    size_t size() {
        return que.size();
    }
};


