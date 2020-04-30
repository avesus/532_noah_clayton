// helper class for a given player to recite its portion of a given play.

#include <broadway/player.h>


#define DO_PING                                                                \
    string ping_msg;                                                           \
    ping_msg += CONTROL_MSG;                                                   \
    ping_msg += "ping";                                                        \
    prepare_send_recvr(pi.recvr,                                               \
                       HEADER_SIZE + ping_msg.length(),                        \
                       (uint8_t *)(&(ping_msg.c_str()[0])));                   \
    reset_recvr_event(pi.recvr, &handle_event, EV_WRITE, WRITING);

// reads config file and inserts all lines to players data structure
void
Player::read(string name, string file) {
    this->lines.clear();
    ifstream ifs(file);
    string   line_str;
    line     l;
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

        if (!(is.eof())) {
            getline(is, line_str);
            l.msg += line_str;
        }

        l.character = name;
        lines.insert(l);
    }
    ifs.clear();
    ifs.seekg(0, ios::beg);
}


// recites each of the lines for this fragment number of the player
void
Player::act(p_info pi) {
    PRINT(HIGH_VERBOSE, "Start Act\n");
    for (auto it = lines.begin(); it != lines.end(); it++) {
        if ((*(pi.progress_state)) == CANCELLED) {
            clear_recvr_outbuf(pi.recvr);
            break;
        }
        active_p->recite(it, pi.frag_num, pi.frags_left, pi.agr_outbuf);
    }
    if ((*(pi.progress_state)) != CANCELLED) {
        active_p->player_exit(READY);
        if (__atomic_sub_fetch((pi.frags_left), 1, __ATOMIC_RELAXED) == 0) {
            DBG_ASSERT((*(pi.agr_outbuf))[0] == CONTENT_MSG,
                       "Error type got corrupted %d -> %d\n",
                       CONTENT_MSG,
                       (*(pi.agr_outbuf))[0]);

#ifdef WITH_SENDBACK
            prepare_send_recvr(pi.recvr,
                               HEADER_SIZE + pi.agr_outbuf->length(),
                               (uint8_t *)(&(pi.agr_outbuf->c_str()[0])));

            reset_recvr_event(pi.recvr, &handle_event, EV_WRITE, WRITING);
#else
            DO_PING;
#endif
            fprintf(stderr, "%s\n", pi.agr_outbuf->c_str());


            active_p->reset_play_state();
            (*(pi.progress_state)) = READY;
            delete pi.agr_outbuf;
            myfree(pi.frags_left);
        }
    }
    else {
        clear_recvr_outbuf(pi.recvr);
        active_p->player_exit(CANCELLED);
        if (__atomic_sub_fetch((pi.frags_left), 1, __ATOMIC_RELAXED) == 0) {
            active_p->reset_play_state();
            (*(pi.progress_state)) = READY;
            delete pi.agr_outbuf;
            myfree(pi.frags_left);
            DO_PING;
        }
    }


    PRINT(HIGH_VERBOSE, "End Act\n");
    // exit when lines are done (as if there is no more to acting
    // than reciting lines... where is the expressiveness!)
}

// while !done (i.e directory finds last fragment is done) continue
// trying to pop parts of the que
void
Player::work(sync_que & q, condition_variable & cv_dir) {
    while (q.done != SHUTDOWN) {
        p_info pi = q.pop();
        if (q.done == SHUTDOWN) {
            break;
        }
        else if (q.done == CANCELLED) {
            usleep(50);
            continue;
        }
        this->active_p = pi.active_play;
        read(pi.name, pi.file);
        act(pi);
        cv_dir.notify_all();
    }
}


// startup a new player thread
void
Player::enter(sync_que & q, condition_variable & cv_dir) {
    thread th([&] { this->work(q, cv_dir); });
    this->t = std::move(th);
}

// quit a player thread
bool
Player::exit() {
    if (this->t.joinable()) {
        t.join();
        return true;
    }
    return false;
}
