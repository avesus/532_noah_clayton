#include "lab2.h"


//For indexing expected arguments for proper usage
enum { 
    program_name = 0,
    input_file = 1,
    expected_num_cmd_args = 2
};


//For likely errors
enum { 
    wrong_num_cmd_args = 0,
    bad_input_file = 1,
    empty_file = 2
};

//Prints usage if arguments are improper
static int usage_main(const char* program_name) {
  std::cout << "Usage: " << program_name << " <input_file>" << std::endl;
  return wrong_num_cmd_args;
}


#define SUCCESS 0
#define FAILURE -1


int verbose = 0;

using namespace std;

int main(int argc, char* argv[]){

  //ensure proper usage
    if (argc != expected_num_cmd_args) {
        return usage_main(argv[program_name]);
    }

    cout << endl;

    //open config file
    string config_file = argv[input_file];

    //create directory
    try {
      Director d(config_file);
      
      d.cue();
    }
    catch (const exception& e){
      cerr << "Error in startup\n" << endl;
      cerr << e.what() << endl;
      return FAILURE;
    }
    catch(...) {
      errdie("Error instantiating director."
	     " Unable to proceed."
	     " Check config file to ensure its valid\n");
    }

    //cue directory to start (i.e start actually reciting play)


    return SUCCESS;
}
