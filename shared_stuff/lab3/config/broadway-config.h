#ifndef _BROADWAY_CONFIG_H_
#define _BROADWAY_CONFIG_H_

//////////////////////////////////////////////////////////////////////
// Basic information for the communication between director/producer

// type of message type information
#define TYPE_TYPE char

// size of type
#define TYPE_SIZE sizeof(char)

// number of different message types
#define NTYPES 4

// index of type into data
#define TYPE_IDX 0

// index where actual data content status
#define DATA_START_IDX 1

// kill msg tells director to shutdown or for producer to free resources
// assosiated with that director
#define KILL_MSG 0x1

// avail is ping from producer to get status of director
#define AVAIL_MSG 0x2

// content message contains the lines of a play
#define CONTENT_MSG 0x3

// control messages are for handling communication within sub states (primarily
// cancelling)
#define CONTROL_MSG 0x4


#define IS_VALID_TYPE(X) (((X) >= 1) && ((X) <= NTYPES))
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// States of director/its plays.

// play is ready
#define READY 0

// play has been cancelled
#define CANCELLED 1

// play recition is currently happening
#define ONGOING 2

// director and all of its plays are shutting down
#define SHUTDOWN 3


//////////////////////////////////////////////////////////////////////
// configuration options

// option to have the director send back recited plays to the producer.
#define WITH_SENDBACK

//optionally include line number in play printout
//#define WITH_LINES

// sleeps during play recitation process to there is time for a human to
// realistically send cancelly
//#define WITH_CANCEL_TIME

#endif
