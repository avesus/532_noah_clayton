
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
  vector<ifstream> configs;
  vector<int> n_players;
  shared_ptr<Play> p;
  vector<shared_ptr<Player>> players;

  mutex m;
  condition_variable cv;

  sync_que frag_que;

 public:

 Director(const string &name, uint32_t min_players=0) : i(name) {
    fprintf(stderr, "in directory constructor");
    if (!i.is_open()) {
      throw std::runtime_error("couldn't open file for reading");
    }

    string buf;
    int max = 0, prev = 0;
    int scene = 0;
    while (getline(i, buf)) {
      fprintf(stderr, "Got line: %s\n", buf.c_str());
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
	  fprintf(stderr, "Adding to configs: %s\n", buf.c_str());
	  configs.push_back(ifstream(buf));
	  n_players.push_back(n_chars);
	}
	scene = 0;
      }
    }
    fprintf(stderr, "got scene/config files: max(%d), n(%lu)\n", max, configs.size());
    p = make_shared<Play>(names);
    for (int j = 0; j < MAX(max, min_players); j++) {
      players.push_back(make_shared<Player>(*p));
      players[j]->enter(this->frag_que, this->cv);
    }
  }

    
  int cue() {
    fprintf(stderr, "Called cue\n");
    unique_lock<mutex> lock(m);
    for (size_t frag = 0; frag < configs.size(); frag++) {
      fprintf(stderr, "doing scene: %lu (%d, %zu) -> %d\n", frag, p->get_on_stage(), frag_que.size(), !(p->get_on_stage() + frag_que.size()));
      cv.wait(lock, [&] { return !(p->get_on_stage() + frag_que.size()); });
      this->p->reset_counter();
      
      p_info p;
      p.frag_num = frag;
      this->p->set_on_stage(n_players[frag]);
      string buf;
      while (getline(this->configs[frag], buf)) {
	fprintf(stderr, "Inside scene(%zu) pushing: %s\n",
		frag,
		buf.c_str());
	if (istringstream(buf) >> p.name >> p.file) {
	  frag_que.push(p);
	}
      }
    }
    fprintf(stderr, "Setting complete!\n");
    frag_que.done = 1;
    frag_que.cv.notify_all();
    for (int j = 0; j < players.size(); j++) {

      players[j]->exit();
    }
    return 0;
  }
};


