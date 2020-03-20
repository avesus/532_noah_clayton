
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


/* Director class (really director & producer). Will open play files
   and based on the script get necessary players (hence producer) to
   recite their lines via the Play class */
class Director {
    
  ifstream i; //ifstream with script file
  vector<string> names; //names of all roles
  vector<ifstream> configs; //will store where each scenes config file is
  vector<int> n_players; //number of players for each scene
  shared_ptr<Play> p; //play class to actually do the scenes
  vector<shared_ptr<Player>> players; //all necessary players

  mutex m;
  condition_variable cv;

  sync_que frag_que; //que for doing leader follower among scenes

 public:

 Director(const string &name, uint32_t min_players=0) : i(name) {
    if (!i.is_open()) {
      throw std::runtime_error("couldn't open file for reading");
    }

    string buf;
    int max = 0, prev = 0;
    int scene = 0;
    while (getline(i, buf)) {

      //case of new scene
      if (strncmp(buf.c_str(), "[scene] ", strlen("[scene] ")) == 0) {
	names.push_back(buf.substr(strlen("[scene] ")));
	scene = 1;
      }
      else {

	//otherwise we expect a config file
	if (!scene) {
	  names.push_back(""); //case where multiple nameless scenes
			       //in one scene
	}
	ifstream config(buf);
	if (config.is_open()) {
	  int n_chars = 0;
	  string buf2;
	  while (getline(config, buf2)) { //get number of players for this scene
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
	  configs.push_back(ifstream(buf)); //stores config file names for this scene
	  n_players.push_back(n_chars);
	}
	scene = 0;
      }
    }

    //creates play/players based on scene file
    p = make_shared<Play>(names); 
    for (int j = 0; j < MAX(max, min_players); j++) { 
      players.push_back(make_shared<Player>(*p));
      players[j]->enter(this->frag_que, this->cv);
    }
  }

  //coordinates the recitation of each play fragment
  int cue() {
    
    //iterates through scene fragments and coordinates players/play
    //based on current/next fragment
    unique_lock<mutex> lock(m);
    for (size_t frag = 0; frag < configs.size() + 1; frag++) {
      cv.wait(lock, [&] { return !(p->get_on_stage() + frag_que.size()); });
      if(frag == configs.size()){
	break;
      }

      //reset play counter between fragments. This resets error
      //checking for missing lines, prints new scene if needed/does
      //any other book keeping
      this->p->reset_counter();

      //set number of expected players for checking skipped line for
      //given fragment
      
      this->p->set_on_stage(n_players[frag]);      
      p_info p;
      p.frag_num = frag;

      string buf;
      while (getline(this->configs[frag], buf)) {
	if (istringstream(buf) >> p.name >> p.file) {
	  //initialize que for worker threads to read from
	  frag_que.push(p);
	}
      }
    }

    //complete all fragments set que to done and notify all worker
    //threads so they can exit properly
    frag_que.done = 1;
    frag_que.cv.notify_all();
    for (int j = 0; j < players.size(); j++) {
      players[j]->exit();
    }
    return 0;
  }
};


