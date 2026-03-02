#ifndef ALLOCATION_H
#define ALLOCATION_H

#define NUM_BLOCKS 80
#define BLOCKSIZE 4096

/* Set the name of block allocation table file.
 * This is necessary to have several examples in the same
 * directory.
 */
void set_block_allocation_table_name( const char* str );

/* Release the memory for the block allocation table file
 * name before exit().
 */
void release_block_allocation_table_name( );

/* Set all the blocks in our simulated disk into an unused
 * state.
 * This function returns 0 in case of success and -1 if the
 * file simulating the blocks cannot be written.
 */
int format_disk();

/* Allocate extent_size consecutive blocks from the available
 * free disk blocks. It does not wrap.
 * Disk blocks are counted from 0 to max.
 * The function can return -1 if consecutive blocks like this
 * are available.
 */
int allocate_block( int extent_size );

/* Free the block with the given ID.
 * This functions returns 0 if the block was freed
 * or -1 if the block with this ID was not allocated
 */
int free_block(int block);

/* This debug function prints the table to stdout. */
void debug_disk();

#endif // ALLOCATION_H
