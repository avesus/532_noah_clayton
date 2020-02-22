#pragma once

#include <set>
#include <string>
#include <thread>

#include "line.h"
#include "play.h"

using namespace std;

class Play;



class Player {
	string name;
	set<line> lines;
	Play& p;
	ifstream ifs;
	thread t;

public:
	Player(Play& p, const string &name, ifstream &ifs) : p(p), name(name), ifs(std::move(ifs)) {}

	void read();

	void act();

	void enter();

	bool exit();
};