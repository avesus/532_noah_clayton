#include "player.h"



//reads config file and inserts all lines to players data structure
void Player::read(string name, string file) {
  this->lines.clear();
    ifstream ifs(file);
	string line_str;
	line l;
	while (getline(ifs, line_str)) {
	  
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

		if(!(is.eof())){
		  getline(is, line_str);
		  l.msg += line_str;
		}

		l.character = name;
		lines.insert(l);
	}
}


//recites each of the lines for this fragment number of the player
void Player::act(size_t frag_num) {
	for (auto it = lines.begin(); it != lines.end(); it++) {
	  
	  p.recite(it, frag_num);
	}

	//exit when lines are done (as if there is no more to acting
	//than reciting lines... where is the expressiveness!)
	p.player_exit();
}

//while !done (i.e directory finds last fragment is done) continue
//trying to pop parts of the que 
void Player::work(sync_que &q, condition_variable &cv_dir) {
    while (!q.done) {
        p_info p = q.pop();
	if(q.done){
	  break;
	}
        read(p.name, p.file);
        act(p.frag_num);
        cv_dir.notify_all();
	
    }
}


//startup a new player thread
void Player::enter(sync_que &q, condition_variable &cv_dir) {
	thread th([&] {
	    this->work(q, cv_dir);
		});
	this->t = std::move(th);
}

//quit a player thread
bool Player::exit() {
	if (this->t.joinable()) {
		t.join();
		return true;
	}
	return false;
}
