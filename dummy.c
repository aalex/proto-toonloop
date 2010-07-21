#include <stdio.h>
#include <jack/jack.h>
#include <stdlib.h>

// jack_client_t *
// xjack_client_new ( const char *name )
// {
//   jack_client_t *client = jack_client_new ( name ) ;
//   if ( ! client ) {
//     fprintf ( stderr , "jack_client_new() failed: %s\n" , name ) ;
//     exit(1) ;
//   }
//   return client ;
// }

int main(int argc, char **argv)
{
    //jack_client_t *client = xjack_client_new("dummy");
    jack_client_t *client = jack_client_open("dummy", JackNullOption, NULL);
    return 0;
}

