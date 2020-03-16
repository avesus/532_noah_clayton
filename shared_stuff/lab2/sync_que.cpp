
#include "sync_que.h"
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <memory>
#include <condition_variable>
#include <mutex>

#include "helpers/opt.h"

void sync_que::push(p_info p) {
    std::lock_guard<mutex> lock(this->m);
    this->que.push(p);
    cv.notify_all();
}


p_info sync_que::pop() {
    std::unique_lock<std::mutex> lock(this->m);
    cv.wait(lock, [&] { return (this->que.size()); });
    p_info ret = que.front();
    que.pop();
    return ret;
}

