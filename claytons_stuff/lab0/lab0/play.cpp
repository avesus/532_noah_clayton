
#include <algorithm>
#include <thread>
#include <unordered_map>

#include "play.h"

using std::endl;


void Play::read_play(istream& f) {
	string char_name;
	line l;
	l.linen = 1;

	while (getline(f, char_name)) {
		if (char_name.size() < 2) {
			continue;
		}
		size_t per_pos; // index of '.'
		if ((per_pos = char_name.rfind('.')) == string::npos) {
			// must be in format "<character name>.\n"
			continue;
		}
		l.character = char_name.substr(0, per_pos);
		while (getline(f, l.msg) && l.msg.size() > 0) {
			(*this) << l;
			l.linen++;
		}
	}
}


void Play::print(ostream &o) {
    // o << name << endl;
	string cur_char = "";
	// lock for reading, nobody can write, but anyone can read
	lock.lock_shared();
    for (auto it = lines.begin(); it != lines.end(); it++) {
		if (it->character != cur_char) {
			if (cur_char != "") {
				// if this is not the first line of the play, input two
				// newlines
				o << endl << endl;
			}
			// print the character's name followed by a period
			cur_char = it->character;
			o << cur_char << '.';
		}
		// print one line of the character's dialog
        o << endl << it->msg;
    }
	lock.unlock_shared();
}

void Play::read_lines(const string& char_name, ifstream& char_lines) {
	string line_str;
	line l;

	while (getline(char_lines, line_str)) {
		istringstream is(line_str);
		if (!(is >> l.linen)) {
			// could not extract line number or first whitespace-delimited
			// string was not a positive number
			continue;
		}

		// need to ignore extra spaces after line number, so extract one
		// word from the line
		if (!(is >> l.msg)) {
			// line is empty
			continue;
		}
		// now get remaining text from the line
		getline(is, line_str);
		l.msg += line_str;

		l.character = char_name;
		lines.insert(l);
	}
}


static string get_file_name(const string& prefix, string char_name) {
	// replace all spaces with underscores
	std::replace(char_name.begin(), char_name.end(), ' ', '_');
	if (prefix.size() == 0) {
		return char_name + ".txt";
	}
	return prefix + "/" + char_name + ".txt";
}


int Play::write_config(const string& config_file) {
	ofstream config(config_file);

	if (!config.is_open()) {
		cout << "unable to open file " << config_file <<
			" for writing" << endl;
		return cant_open_file;
	}

	// find directory that out file will be placed in so all other
	// config files can be placed there too
	string out_dir;
	size_t last_slash;
	if ((last_slash = config_file.rfind('/'))  != string::npos ||
		(last_slash = config_file.rfind('\\')) != string::npos) {
		out_dir = config_file.substr(0, last_slash);
	}


	// will contain map of characters' names to corresponding
	// output files containing their lines
	unordered_map<string, ofstream> char_files;

	for (auto it = lines.begin(); it != lines.end(); it++) {
		auto in_map = char_files.find(it->character);
		if (in_map == char_files.end()) {
			// character has not been seen yet, try creating
			// an output file for them

			string out_name = get_file_name(out_dir, it->character);
			const ofstream& out = char_files[it->character] = ofstream(out_name);

			if (!out.is_open()) {
				cout << "unable to create file " << out_name << " for writing" << endl;
				return cant_open_file;
			}

			in_map = char_files.find(it->character);

			cout << "added " << it->character << endl;
		}

		in_map->second << it->linen << " " << it->msg << endl;
	}

	config << this->name << endl;
	for (auto it = char_files.begin(); it != char_files.end(); it++) {
		config << it->first << " " << get_file_name("", it->first) << endl;
	}

	return success;
}


