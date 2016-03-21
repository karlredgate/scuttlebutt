
#ifndef _SCTLBT_PORT_H_
#define _SCTLBT_PORT_H_

struct ring;

struct ring * create_receiver( int index );
int create_transmitter( int index );
void receive_loop( struct ring * );

#endif

/* vim: set autoindent expandtab sw=4 : */
