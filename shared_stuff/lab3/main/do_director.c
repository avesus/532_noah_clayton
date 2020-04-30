//////////////////////////////////////////////////////////////////////
//Main to run producer for Lab 3 in CSE 532

#include "do_director.h"


//#define ARG_PARSER_ON
#define ARG_PARSER_OFF

#if defined ARG_PARSER_ON && defined ARG_PARSER_OFF
    #undef ARG_PARSER_ON
#endif

#if !defined ARG_PARSER_ON && !defined ARG_PARSER_OFF
    #define ARG_PARSER_OFF
#endif

#ifdef ARG_PARSER_OFF
    #define MIN_EXPEC_NARGS 5
    #define PROG_NAME_IDX   0
    #define PORT_IDX        1
    #define IP_IDX          2
    #define MIN_THREAD_IDX  3
    #define FILE_START_IDX  4
#endif


int32_t  verbose     = 0;
uint32_t min_threads = 0;
uint32_t portno      = DEFAULT_PORTNO;
char *   ip_addr     = DEFAULT_SERVER_IP;


#ifdef ARG_PARSER_ON

char * files = NULL;

    #define NSTRINGS 2
void * to_free[NSTRINGS];

// clang-format off
#define Version "0.1"
static ArgOption args[] = {
  // Kind,        Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindOption,   Integer, 		"-p", 	    0,     &portno, 		"Set port number" },
  { KindOption,   Integer, 		"-t", 	    0,     &min_threads,	"Set min threads" },
  { KindOption,   Integer, 		"--ip",	    0,     &ip_addr, 		"Set ip address of server" },
  { KindOption,   String, 		"-f", 	    0,     &files,   		"Set play files as csv" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
// clang-format on

static ArgDefs argp = { args, "Main", Version, NULL };
#endif


void
usage(char * pname) {
    die("Usage: %s <Integer: port number> <String: ip address> <Integer: min "
        "threads> <String: File 0> ... <String: File N>\n",
        pname);
}


int
main(int argc, char ** argv) {

    srand(argc);
    INIT_DEBUGGER;

#ifdef ARG_PARSER_ON
    files      = (char *)mycalloc(BIG_BUF_LEN, sizeof(char));
    ip_addr    = (char *)mycalloc(SMALL_BUF_LEN, sizeof(char));
    to_free[0] = files;
    to_free[1] = ip_addr;


    progname = argv[0];

    ArgParser * ap = createArgumentParser(&argp);
    if (parseArguments(ap, argc, argv)) {
        errdie("Error parsing arguments");
    }
    freeCommandLine();
    freeArgumentParser(ap);
    std::string str_files = files;

    // my arg parser doesn't do so well with space seperated arguments so need
    // to replace commas with spaces
    for (size_t i = 0; i < str_files.length(); i++) {
        if (str_files[i] == ',') {
            str_files[i] = ' ';
        }
    }
#endif
#ifdef ARG_PARSER_OFF
    if (argc < MIN_EXPEC_NARGS) {
        usage(argv[PROG_NAME_IDX]);
    }
    char * end = argv[PORT_IDX];
    portno     = strtol(argv[PORT_IDX], &end, 10);
    if (end == argv[PORT_IDX]) {
        usage(argv[PROG_NAME_IDX]);
    }

    end         = argv[MIN_THREAD_IDX];
    min_threads = strtol(argv[MIN_THREAD_IDX], &end, 10);
    if (end == argv[MIN_THREAD_IDX]) {
        usage(argv[PROG_NAME_IDX]);
    }

    char * ip_addr = argv[IP_IDX];

    std::string str_files = argv[FILE_START_IDX];
    for (int32_t i = FILE_START_IDX + 1; i < argc; i++) {
        str_files += " ";
        str_files += argv[i];
    }
#endif


    Director * d = new Director(str_files, min_threads, ip_addr, portno);
    d->start_directing();


    FREE_DEBUGGER;

#ifdef ARG_PARSER_ON
    for (int32_t f = 0; f < NSTRINGS; f++) {
        myfree(to_free[f]);
    }
#endif
    return 0;
}
