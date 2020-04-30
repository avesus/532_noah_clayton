//////////////////////////////////////////////////////////////////////
//Main to run producer for Lab 3 in CSE 532


#include "do_producer.h"

//#define ARG_PARSER_ON
#define ARG_PARSER_OFF

#if defined ARG_PARSER_ON && defined ARG_PARSER_OFF
    #undef ARG_PARSER_ON
#endif

#if !defined ARG_PARSER_ON && !defined ARG_PARSER_OFF
    #define ARG_PARSER_OFF
#endif

#ifdef ARG_PARSER_OFF
    #define EXPEC_NARGS   2
    #define PROG_NAME_IDX 0
    #define PORT_IDX      1
#endif

int32_t verbose = 0;
uint32_t portno = DEFAULT_PORTNO;

#ifdef ARG_PARSER_ON
    // clang-format off
#define Version "0.1"
static ArgOption args[] = {
  // Kind,        Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindOption,   Integer, 		"-p", 	    0,     &portno, 		"Set port number" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
// clang-format on

static ArgDefs argp = { args, "Main", Version, NULL };
#endif

void
usage(char * pname) {
    die("Usage: %s <Integer: port number>\n", pname);
}

int
main(int argc, char ** argv) {
    srand(argc);
    INIT_DEBUGGER;

#ifdef ARG_PARSER_ON
    progname       = argv[0];
    ArgParser * ap = createArgumentParser(&argp);
    if (parseArguments(ap, argc, argv)) {
        errdie("Error parsing arguments");
    }
    freeCommandLine();
    freeArgumentParser(ap);
#endif
#ifdef ARG_PARSER_OFF
    if (argc != EXPEC_NARGS) {
        usage(argv[PROG_NAME_IDX]);
    }
    char *   end    = argv[PORT_IDX];
    portno = strtol(argv[PORT_IDX], &end, 10);
    if (end == argv[PORT_IDX]) {
        usage(argv[PROG_NAME_IDX]);
    }
#endif

    producer * p = new producer(0, DEFAULT_SERVER_IP, portno);
    p->produce();


    FREE_DEBUGGER;
    return 0;
}
