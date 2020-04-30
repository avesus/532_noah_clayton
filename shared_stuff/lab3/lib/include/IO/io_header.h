// Struct that will server as format for all IO. Very simply contains a length
// field followed by variable amount of data. As well some basic defines/macros
// so that the struct can be changed without affecting the rest of the program.

#ifndef _IO_HEADER_H_
#define _IO_HEADER_H_

#include <stdint.h>

typedef struct io_data {
    uint32_t length;
    char     data[0];
} io_data_t;


// sizeof length field
#define HEADER_TYPE    uint32_t
#define HEADER_SIZE    sizeof(HEADER_TYPE)
#define IO_DATA_LEN(X) ((io_data *)(X))->length

#endif
