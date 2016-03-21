
#include <net/if.h>

#include "port.h"

int main( int argc, char **argv ) {
    int index = if_nametoindex( "switch0" );
    struct ring *ring = create_receiver( index );
    receive_loop( ring );
    return 0;
}

/* vim: set autoindent expandtab sw=4 : */
