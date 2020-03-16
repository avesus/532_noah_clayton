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





#define SUCCESS 0
#define FAILURE -1


int verbose = 0;

using namespace std;

int main(int argc, char* argv[]){
    if (argc != expected_num_cmd_args) {
        return usage_main(argv[program_name]);
    }

    string config_file = argv[input_file];
    fprintf(stderr, "opening: %s", config_file.c_str());
    Director d(config_file);
    d.cue();

    return SUCCESS;
}
