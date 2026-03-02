#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <errno.h>

#include "block_allocation.h"

/* Read the block allocation table from file into memory, if such a
 * file exists.
 */
static char* read_table( );

/* Write the block allocation from memory into a file, if such a
 * file can be written.
 */
static int write_table( );

/* Called when the program terminates without error, and writes
 * the block allocation table to its file in that case.
 * The function is not called directly but through atexit().
 */
void save_and_release_block_allocation_table( );

/* The name of the file that contains our block allocation table
 * that simulates the used disk.
 * We make the variable static to hide it from other C files.
 */
static char* file_name = NULL;

static char* block_allocation_table = NULL;

void set_block_allocation_table_name( const char* str )
{
    if( file_name != NULL )
    {
        fprintf( stderr, "Cannot set %s as block allocation table name.\n"
                         "The name of the block_allocation_table has already been set to %s\n",
                         str, file_name );
        exit( -1 );
    }

    file_name = strdup( str );

    block_allocation_table = read_table();

    atexit( &save_and_release_block_allocation_table );
}

void save_and_release_block_allocation_table( )
{
    if( file_name )
    {
        if( block_allocation_table )
        {
            write_table( );
            free( block_allocation_table );
        }

        free( file_name );
    }
}


static char* read_table( )
{
    if( file_name == NULL )
    {
        fprintf( stderr, "Failed to set the name of the block allocation table file.\n" );
        exit( -1 );
    }

    char* table = malloc( NUM_BLOCKS );
    if( table == NULL )
    {
        fprintf( stderr, "Failed to allocate %d bytes\n", NUM_BLOCKS );
        return NULL;
    }

    FILE* f = fopen( file_name, "r" );
    if( !f )
    {
        fprintf( stderr, "Failed to open file %s for reading\n", file_name );
        perror("Reason:");
        free( table );
        return NULL;
    }

    int num_read = fread( table, 1, NUM_BLOCKS, f );
    if( num_read != NUM_BLOCKS )
    {
        fprintf( stderr, "Failed to load %d block entries from disk\n", NUM_BLOCKS );
        perror("Reason:");
        fclose(f);
        free( table );
        return NULL;
    }
    fclose( f );

    return table;
}

static int write_table( )
{
    if( file_name == NULL )
    {
        fprintf( stderr, "Failed to set the name of the block allocation table file.\n" );
        exit( -1 );
    }

    if( block_allocation_table == NULL )
    {
        fprintf( stderr, "Block allocation table has not been created in memory before writing.\n" );
        return -1;
    }

    FILE* f = fopen( file_name, "w" );
    if( !f )
    {
        fprintf( stderr, "Failed to open file %s for writing\n", file_name );
        perror("Reason:");
        return -1;
    }
    int num = fwrite( block_allocation_table, 1, NUM_BLOCKS, f );
    if( num != NUM_BLOCKS )
    {
        fprintf( stderr, "Failed to write %d bytes to %s, ", NUM_BLOCKS, file_name);
        fprintf( stderr, "fwrite returned %d\n", num );
        perror("Reason:");
        return-1;
    }
    fclose( f );
    return 0;
}

int format_disk()
{
    if( file_name == NULL )
    {
        fprintf( stderr, "Failed to set the name of the block allocation table file.\n" );
        exit( -1 );
    }

    int error = unlink( file_name );

    if( error == 0 || ( error == -1 && errno == ENOENT ) )
    {
        if( block_allocation_table ) free( block_allocation_table );

        /* We want to set all NUM_BLOCK chars to 0, convenient to use
         * calloc.
         */
        block_allocation_table = calloc( NUM_BLOCKS, 1 );
        if( block_allocation_table == NULL )
        {
            fprintf( stderr, "Failed to allocate %d bytes\n", NUM_BLOCKS );
            return -1;
        }

        int retval = write_table( );
        return retval;
    }
    fprintf( stderr, "Failed to remove existing file %s (%s)\n", file_name, strerror(errno) );
    perror("reason:");
    return -1;
}

int allocate_block( int extent_size )
{
    if( extent_size == 0 )
    {
        // outside the permitted range
        fprintf( stderr, "Programming error: Trying to allocate extent that is 0 blocks long.\n" );
        return -1;
    }

    if( extent_size < 0 )
    {
        // outside the permitted range
        fprintf( stderr, "Programming error: Trying to allocate extent of negative size. Unrecoverable.\n" );
        exit( -1 );
    }

    if( extent_size > 4 )
    {
        // outside the permitted range
        return -1;
    }

    if( block_allocation_table == NULL )
        block_allocation_table = read_table( );

    if( block_allocation_table == NULL ) 
    {
        return -1;
    }

    /* first fit algorithm */
    for( int i=0; i<NUM_BLOCKS; i++ )
    {
        /* extent_size blocks in a row that are free? */
        int found_blk = 1;
        for( int j=0; j<extent_size; j++ )
            if( ( i+j>=NUM_BLOCKS ) || ( block_allocation_table[i+j] != 0 ) )
            {
                found_blk = 0;
                break;
            }
        /* If not, continue to next i */
        if( found_blk == 0 ) continue;

        /* Found extent_size unused contiguous blocks.
         * Allocate them. */
        for( int j=0; j<extent_size; j++ )
            block_allocation_table[i+j] = 1;

        return i;
    }
    return -1;
}

int free_block( int block )
{
    if( block < 0 || block >= NUM_BLOCKS )
    {
        fprintf( stderr, "Block number %d is not in range\n", block );
        return -1;
    }

    if( block_allocation_table == NULL )
        block_allocation_table = read_table( );

    if( block_allocation_table == NULL ) 
        return -1;

    if( block_allocation_table[block] != 1 )
    {
        fprintf( stderr, "Block %d was not allocated\n", block );
        return -1;
    }

    block_allocation_table[block] = 0;

    return 0;
}

void debug_disk( )
{
    if( block_allocation_table == NULL )
        block_allocation_table = read_table( );

    if( block_allocation_table == NULL )
    {
        fprintf( stderr, "Failed to read block allocation table\n" );
        return;
    }

    printf("Blocks recorded in the block allocation table:");
    for( int i=0; i<NUM_BLOCKS; i++ )
    {
        if( i % 20 == 0 ) printf("\n%03d: ", i);
        printf("%d", block_allocation_table[i] );
    }
    printf("\n\n");
}

