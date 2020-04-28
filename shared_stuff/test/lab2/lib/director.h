#ifndef _DIRECTOR_H_
#define _DIRECTOR_H_

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
  Director(const string &name);
  Director(const string &name, uint32_t min_players);
  //coordinates the recitation of each play fragment
  int cue();
};


#endif
