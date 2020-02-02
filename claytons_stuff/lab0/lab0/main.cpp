
#include <algorithm>
#include <vector>

#include "play.h"


enum {
	program_name,
	input_file,
	expected_num_cmd_args
};

enum {
	wrong_num_cmd_args = 0,
	bad_input_file,
	empty_file
};

int usage(const char* program_name) {
	cout << "Usage: " << program_name << " <input_file>" << endl;
	return wrong_num_cmd_args;
}



static string get_file_name(const string& prefix, string char_name) {
	// replace all spaces with underscores
	std::replace(char_name.begin(), char_name.end(), ' ', '_');
	if (prefix.size() == 0) {
		return char_name;
	}
	return prefix + "/" + char_name;
}


int main(int argc, char* argv[]) {

	if (argc != expected_num_cmd_args) {
		return usage(argv[program_name]);
	}

	string config_file = argv[input_file];
	ifstream i(config_file);

	if (!i.is_open()) {
		cout << "Unable to open file " << argv[input_file] <<
			" for reading" << endl;
		return bad_input_file;
	}

	string title, char_name, file_name;

	if (!getline(i, title)) {
		cout << "Input file was empty" << endl;
		return empty_file;
	}
	Play p(title);

	// find directory that out file will be placed in so all other
	// config files can be placed there too
	string out_dir;
	size_t last_slash;
	if ((last_slash = config_file.rfind('/')) != string::npos ||
		(last_slash = config_file.rfind('\\')) != string::npos) {
		out_dir = config_file.substr(0, last_slash);
	}

	vector<thread> threads;

	while (getline(i, title)) {
		string char_name_line;

		size_t last_sp = title.rfind(' ');
		if (last_sp == string::npos || last_sp == 0) {
			cout << "Badly formatted line \"" << title << "\", expect "
				"<character name> <file with character's lines>" << endl;
		}
		char_name = title.substr(0, last_sp);
		file_name = title.substr(last_sp + 1);

		// file_name is relative to out_dir
		file_name = get_file_name(out_dir, file_name);

		ifstream f(file_name);
		if (!f.is_open()) {
			cout << "Unable to open character file " << file_name << endl;
			continue;
		}

		// spawn a new thread to process this character's lines
		threads.push_back(thread(&Play::read_lines, std::ref(p), char_name, std::move(f)));
	}

	for (auto it = threads.begin(); it != threads.end(); it++) {
		try {
			it->join();
		}
		catch (...) {}
	}

	p.print(cout);

	return success;
}
