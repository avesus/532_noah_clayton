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
  config_file.open(argv[1], std::fstream::in);
  if(!config_file){
    cout << "Couldn't open config file" << endl;
    return FAILURE;
  }
  std::string play_name;
  if(!getline(config_file, play_name, '\n')){
    cout << "Couldn't read play name" << endl;
    return FAILURE;
  }

  play p(play_name);
  
  vector<worker_thread*> workers;

  while(!config_file.eof()){
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
  if(!workers.size()){
    cout << "No character names" << endl;
    return FAILURE;
  }


  for(int i=0;i<workers.size();i++){
    workers[i]->start();
  }
  for(int i=0;i<workers.size();i++){
    workers[i]->stop();
  }



  p.print(cout);
  return SUCCESS;
}
