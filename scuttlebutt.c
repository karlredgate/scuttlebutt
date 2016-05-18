
#include <stdlib.h>
#include <string.h>
#include <net/if.h>

#include "port.h"

int
debug( int argc, char **argv ) {
    if ( argc == 1 ) return 0;
    if ( strcmp(argv[1], "--debug") != 0 ) return 0;
    return 1;
}

int main( int argc, char **argv ) {
    int index = if_nametoindex( "switch0" );
    struct ring *ring = create_receiver( index );

    if ( debug(argc, argv) ) {
        exit( 0 );
    }

    receive_loop( ring );
    return 0;
}

/* vim: set autoindent expandtab sw=4 : */
