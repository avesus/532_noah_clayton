
#include <iterator>
#include <vector>

#include "..\lab0\play.h"



enum {
	program_name,
	frag_file,
	out_file,
	play_name,
	min_num_cmd_args
};

enum {
	wrong_num_cmd_args = 1,
	bad_input_file,
	empty_file
};

int usage(const char* program_name) {
	cout << "Usage: " << program_name << " <play_frag_file> "
		"<output_file_name> <play name...>" << endl;
	return wrong_num_cmd_args;
}


int main(int argc, char* argv[]) {

	if (argc < min_num_cmd_args) {
		return usage(argv[program_name]);
	}

	ifstream frag(argv[frag_file]);

	if (!frag.is_open()) {
		cout << "Unable to open file " << argv[frag_file] <<
			" for reading" << endl;
		return bad_input_file;
	}

	// get play title
	ostringstream t;
	t << argv[play_name];
	for (int i = play_name + 1; i < argc; i++) {
		t << " " << argv[i];
	}
	string title = t.str();

	// construct play from fragment file
	Play p(title, frag);

	// print play
	//p.print(cout);

	// write config files from Play
	p.write_config(argv[out_file]);

	return success;
}
