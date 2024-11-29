#ifndef SERVER_H
#define SERVER_H

// Stdlib
#include <stdint.h>

typedef void (*byte_handler_fn)(uint8_t);

void
tcp_server_run(void* parameters);

#endif // SERVER_H