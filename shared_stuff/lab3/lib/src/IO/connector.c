// Struct that runs a base event loop for a single receiver that is connected to
// a socket. The connector (as its name might suggest) can connect to a net
// socket and handle IO over that socket. Each connector is meant to only handle
// a single connection.

#include <IO/connector.h>

static void
default_sigint_handler(const int32_t sig) {
    DBG_ASSERT(
        sig == SIGINT,
        "Error sigint somehow called without SIGINT...\nClearly I messed up\n");

    fprintf(stderr, "SIGINT handler evoked\n");
    exit(0);
}





#ifdef HAS_STDIN
static void *
handle_stdin_command(void * arg, io_data * data_buf) {
    return NULL;
}
#endif

static int32_t
do_connect(const char * ip_addr, uint32_t portno) {
    struct sockaddr_in addr;
    int                net_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port   = htons(portno);

    inet_aton(ip_addr, &addr.sin_addr);

    if (connect(net_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) ==
        (-1)) {
        errdie(
            "Couldnt connect to server at\n\t"
            "ip  : %s\n\t"
            "port: %d\n",
            ip_addr,
            portno);
    }
    
    return net_fd;
}


static void *
handle_command(void * arg, io_data * data_buf) {
    myfree(data_buf);
    return NULL;
}


connector_t *
init_connector(const char * ip_addr, uint32_t portno) {

    connector_t * new_connect = (connector_t *)mycalloc(1, sizeof(connector_t));

    new_connect->ip_addr = ip_addr;
    new_connect->portno  = portno;

    new_connect->connect_base = event_base_new();
    if (new_connect->connect_base == NULL) {
        errdie("Couldn't initialize connector event base\n");
    }

    int32_t net_fd         = do_connect(ip_addr, portno);
    new_connect->net_recvr = init_recvr(net_fd,
                                        0,
                                        NET_IDX,
                                        (void *)new_connect,
                                        new_connect->connect_base,
                                        &handle_command);
#ifdef HAS_STDIN
    new_connect->stdin_recvr = init_stdin_recvr(0,
                                                (void *)new_connect,
                                                new_connect->connect_base,
                                                &handle_stdin_command);
#endif

    return new_connect;
}

void
free_connector(connector_t * connect) {
    if (event_base_loopbreak(connect->connect_base) == (-1)) {
        errdie("Error breaking connector event loop\n");
    }
    free_recvr(connect->net_recvr);
#ifdef HAS_STDIN
    free_recvr(connect->stdin_recvr);
#endif
    myfree(connect);
}

void
start_connect(connector_t * connect) {
    if (event_base_loop(connect->connect_base, EVLOOP_NO_EXIT_ON_EMPTY) != 0) {
        errdie("Error running connector event loop\n");
    }
}
