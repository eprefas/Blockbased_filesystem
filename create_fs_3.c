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

    /* format_disk() makes sure that the simulated
     * disk is empty. It creates a file named
     * block_allocation_table that contains only
     * zeros. */
    format_disk();

    /* debug_disk() write the current content of the
     * block_allocation_table that simulates whether
     * blocks on disk contain file data (1) or not (0).
     */
    debug_disk();

    struct inode* root      = create_dir( NULL, "/" );
    struct inode* dir_root  = create_dir( root, "root" );
    struct inode* dir_home  = create_dir( root, "home" );
    struct inode* dir_guest = create_dir( dir_home, "guest" );
    struct inode* dir_user  = create_dir( dir_home, "user" );
    struct inode* dir_down  = create_dir( dir_user, "Download" );
    create_dir( dir_home, "print" );

    create_file( dir_root,  "bashrc", 1, 100 );
    create_file( dir_root,  "profile", 1, 100 );
    create_file( dir_guest, "bashrc", 1, 100 );
    create_file( dir_guest, "profile", 1, 100 );
    create_file( dir_user,  "bashrc", 1, 100 );
    create_file( dir_user,  "profile", 1, 100 );
    create_file( dir_down,  "oblig2", 1, 163033 );

    debug_fs( root );
    debug_disk();

    save_inodes( mft_name, root );

    fs_shutdown( root );

    printf( "++++++++++++++++++++++++++++++++++++++++++++++++\n" );
    printf( "+ All inodes structures have been\n" );
    printf( "+ deleted. The inode info is stored in\n" );
    printf( "+ %s\n", mft_name );
    printf( "+ The allocated file blocks are stored in\n" );
    printf( "+ %s\n", bat_name );
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++\n" );
}

