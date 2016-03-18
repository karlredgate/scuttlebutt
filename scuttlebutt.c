
#include <net/if.h>

#include "port.h"

int main( int argc, char **argv ) {
    int index = if_nametoindex( "sink" );
    create_receiver( index );
    return 0;
}

/* vim: set autoindent expandtab sw=4 : */
