#include "lab2.h"

#include "director.h"

enum {
    program_name = 0,
    input_file = 1,
    expected_num_cmd_args = 2
};

enum {
    wrong_num_cmd_args = 0,
    bad_input_file = 1,
    empty_file = 2
};

static int usage_main(const char* program_name) {
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


#define SUCCESS 0
#define FAILURE -1


int verbose = 0;

using namespace std;

int main(int argc, char* argv[]){
    if (argc != expected_num_cmd_args) {
        return usage_main(argv[program_name]);
    }

    string config_file = argv[input_file];

    Director d(config_file);
    d.cue();

    return SUCCESS;
}
