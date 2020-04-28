#include "play.h"

using std::endl;

//recites the play. Coordinates all players via line_counter and
//within a given line via n_passed and on_stage.
void Play::recite(set<line>::iterator& it, int32_t expec_fragments_lines) {
    unique_lock<mutex> l(m);
    while ((line_counter < it->linen && //case where potentiall good to recite
                this->scene_fragment_counter == expec_fragments_lines) ||
	   (this->scene_fragment_counter < expec_fragments_lines)
          ){

        n_passed++;


	//case where no player for the current line
        if (n_passed == on_stage) {
            // nobody could read this line, need to skip it
            cerr << endl << "****** line " << line_counter << " skipped ******";
            line_counter++;
            n_passed = 0;
            cv.notify_all();
            continue;
        }
        cv.wait(l);
    }
     //error case
    if (line_counter != it->linen ||
            this->scene_fragment_counter > expec_fragments_lines) {
        cerr << endl << "****** line " << it->linen << " said by "
            << it->character << " skipped fragment ******";
        this->it++;
        cv.notify_all();
        return;
    }

    //change of character
    if (cur_char != it->character) { 
        if (cur_char != "") {
            cout << endl << endl;
        }
        cur_char = it->character;
        cout << it->character << '.';
    }

    //actually printing the line
    cout << endl << it->linen << ": "<< it->msg;
    line_counter++;
    n_passed = 0;
    cv.notify_all();
}

