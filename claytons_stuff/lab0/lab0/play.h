#ifndef _PLAY_H
#define _PLAY_H

#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;


enum {
	success,
	cant_open_file
};

struct line {
	unsigned linen;
	string character;
	string msg;

	bool operator<(const line& l) const {
		return linen < l.linen;
	}
};


class Play {
    string name;
    set<line> lines;
	shared_mutex lock;

	/*
	 * reads a play from f into a Play object
	 */
	void read_play(istream& f);

public:
    Play(const string &name): name(name) {}

	/*
	 * construct a play from a play fragment file
	 */
	Play(const string& name, istream& f) : name(name) {
		read_play(f);
	}

    Play &operator<<(const line &l) {
		// lock for writing (nobody else can read or write)
		lock.lock();
        lines.insert(l);
		lock.unlock();
		return *this;
    }

    void print(ostream &o);

	/*
	 * reads a character's lines into the Play
	 */
	void read_lines(const string& char_name, ifstream& char_lines);


	/*
	 * given the name of the config file to create, makes the config
	 * file along with all the individual character files
	 */
	int write_config(const string& config_file);

};

#endif /* _PLAY_H */
