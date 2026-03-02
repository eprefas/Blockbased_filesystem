
#ifndef ALLOCATION_H
#define ALLOCATION_H

#define NUM_BLOCKS 80
#define BLOCKSIZE 4096

/* Set the filename used to persist the block allocation table. */
void set_block_allocation_table_name(const char* str);

/* Release memory for the stored filename (if any). */
void release_block_allocation_table_name();

/* Create or zero the block allocation file on disk. */
int format_disk();

/* Allocate a contiguous extent of blocks (returns start index or -1). */
int allocate_block(int extent_size);

/* Free a single block (returns 0 on success). */
int free_block(int block);

/* Print the block allocation table to stdout. */
void debug_disk();

#endif
