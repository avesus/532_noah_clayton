#include "lab0.h"


#define SUCCESS 0
#define FAILURE -1

#define NUM_ARGS 2
  
int verbose = 0;


using namespace std;

int main(int argc, char* argv[]){
  if(argc!=NUM_ARGS){
    cout << "Usage: " << argv[0] << " <config_file>" << endl;
    return FAILURE;
  }
  std::fstream config_file;

  //opening play class and getting play name
  std::string play_name;
  try {
    config_file.open(argv[1], std::fstream::in);
    if(!config_file){
      cerr << "Couldn't open config file" << endl;
      return FAILURE;
    }
    if(!getline(config_file, play_name, '\n')){
      cerr << "Couldn't read play name" << endl;
      return FAILURE;
    }
  }
  catch (const std::exception& e){
    cerr << "Error in startup\n" << endl;
    cerr << e.what() << endl;
    return FAILURE;
  }
  catch (...) {
    cerr << "Caught unknown exception during play setup" << endl;
    return FAILURE;
  }

  //create play class
  play p(play_name);

  //now time to start workers
  vector<worker_thread*> workers;


  while(!config_file.eof()){
    try {
      std::string line;
      if(!getline(config_file, line, '\n')){
	continue;
      }
      stringstream ss(line);
      std::string* cname = new string, fname;
      ss >> (*cname);
      if(cname->empty()){
	continue;
      }
      ss >> fname;
      if(fname.empty()){
	continue;
      }
      fstream* name_file = new fstream;
      name_file->open(fname.c_str(), std::fstream::in);
      if(!name_file->is_open()){
	continue;
      }
      worker_thread* w = new worker_thread(p, *cname, *name_file);
      workers.push_back(w);
    }
    catch (const std::exception& e){
      cerr << "Error during thread startup" << endl;
      cerr << e.what() << endl;
      return FAILURE;
    }
    catch (...) {
      cerr << "Caught unknown exception during worker setup" << endl;
      return FAILURE;
    }
  }

  if(!workers.size()){
    cout << "No character names" << endl;
    return FAILURE;
  }
  try{
  for(unsigned int i=0;i<workers.size();i++){
    workers[i]->start();
  }
  for(unsigned int i=0;i<workers.size();i++){
    workers[i]->stop();
  }
  p.print(cout);
  }
    catch (const std::exception& e){
      cerr << "Error running workers" << endl;
      cerr << e.what() << endl;
      return FAILURE;
    }
    catch (...) {
      cerr << "Caught unknown exception running workers" << endl;
      return FAILURE;
    }
  return SUCCESS;
}
