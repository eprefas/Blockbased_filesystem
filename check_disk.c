#include "block_allocation.h"

#include <stdio.h>
#include <stdlib.h>

int alloc_recurse( int to_allocate, int blksz )
{
    if( to_allocate < 1 ) return 0;

    if( to_allocate < blksz ) blksz = to_allocate;

    int val = allocate_block( blksz );
    if( val == -1 )
    {
        if( blksz == 1 )
            return -1;
        else
            return alloc_recurse( to_allocate, blksz-1 );
    }
    else
    {
        int lowval = alloc_recurse( to_allocate-blksz, blksz );
        if( lowval == 0 )
            return 0;

        for( int i=0; i<blksz; i++ )
            free_block(val+i);
        return -1;
    }
}

void alloc_n(int n)
{
    int blksz = 4;
    if( alloc_recurse( n, 4 ) == -1 )
    {
        fprintf(stderr, "Failed to allocate %d blocks\n", n);
    }
}

int main( int argc, char* argv[] )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Usage: %s BAT\n"
                         "       where\n"
                         "       BAT is the name of the block allocation table\n"
                         , argv[0] );
        exit( -1 );
    }

    char* bat_name = argv[1];

    set_block_allocation_table_name( bat_name );

    format_disk();
    debug_disk();
        
    alloc_n(1);
    debug_disk();

    alloc_n(10);
    debug_disk();

    free_block(3);
    debug_disk();

    alloc_n(4);
    debug_disk();

    alloc_n(5);
    debug_disk();

    alloc_n(31);
    debug_disk();

    alloc_n(100);
    debug_disk();

    alloc_n(2);
    debug_disk();
}

