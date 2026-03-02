#include "inode.h"
#include "block_allocation.h"

#include <stdio.h>

int main( int argc, char* argv[] )
{
    if( argc != 3 )
    {
        fprintf( stderr, "Usage: %s MFT BAT\n"
                         "       where\n"
                         "       MFT is the name of the master_file_table\n"
                         "       BAT is the name of the block allocation table\n"
                         , argv[0] );
        exit( -1 );
    }

    char* mft_name = argv[1];
    char* bat_name = argv[2];

    set_block_allocation_table_name( bat_name );

    int error;

    /* debug_disk() write the current content of the
     * block_allocation_table that simulates whether
     * blocks on disk contain file data (1) or not (0).
     */
    debug_disk();

    printf("===================================\n");
    printf("= Load all inodes from the file   =\n");
    printf("= master_file_table               =\n");
    printf("===================================\n");
    struct inode* root = load_inodes( mft_name );
    debug_fs( root );

    fs_shutdown( root );

    return 0;
}

