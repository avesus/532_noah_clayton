
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
	  else{
	    max = n_chars;
	  }
	  configs.push_back(ifstream(buf));
	  n_players.push_back(n_chars);
	}
	scene = 0;
      }
    }
    p = make_shared<Play>(names);
    for (int j = 0; j < MAX(max, min_players); j++) {
      players.push_back(make_shared<Player>(*p));
      players[j]->enter(this->frag_que, this->cv);
    }
  }

    
  int cue() {
    unique_lock<mutex> lock(m);
    for (size_t frag = 0; frag < configs.size() + 1; frag++) {
      cv.wait(lock, [&] { return !(p->get_on_stage() + frag_que.size()); });
      if(frag == configs.size()){
	break;
      }

	
      this->p->reset_counter();
      this->p->set_on_stage(n_players[frag]);      
      p_info p;
      p.frag_num = frag;

      string buf;
      while (getline(this->configs[frag], buf)) {
	if (istringstream(buf) >> p.name >> p.file) {
	  frag_que.push(p);
	}
      }
    }

    frag_que.done = 1;
    frag_que.cv.notify_all();
    for (int j = 0; j < players.size(); j++) {
      players[j]->exit();
    }
    return 0;
  }
};


