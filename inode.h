#ifndef INODE_H
#define INODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*******************************************************************************
 * BEGIN: ADD YOUR OWN STRUCT AND MACROS BELOW HERE
 ******************************************************************************/

struct Extent
{
    uint32_t blockno;
    uint32_t extent;
};

/*******************************************************************************
 * END: ADD YOUR OWN STRUCT AND MACROS ABOVE HERE
 ******************************************************************************/

/* This is the inode structure as described in the
 * assignment.
 * It is mostly straightforward, but keep in mind
 * that the dynamically allocated array entries
 * contains values that you must interpret as pointers
 * when is_directory==1 and that you must interpret
 * as block numbers when is_directory==0.
 */
struct inode
{
	uint32_t   id;
	char*      name;
	char       is_directory;
	char       is_readonly;
	uint32_t   filesize;
	uint32_t   num_entries;
	uintptr_t* entries;
};

/* Create a file below the inode parent. Parent must
 * be a directory. The size of the file is size_in_bytes,
 * and create_file calls the allocate_block() function
 * enough number of times to reserve blocks in the simulated
 * disk to store all of these bytes.
 * Returns a pointer to file's inodes.
 */
struct inode* create_file( struct inode* parent,
                           const char* name,
                           char readonly,
                           int size_in_bytes );

/* Create a directory below the inode parent. Parent must
 * be a directory.
 * Returns a pointer to file's inodes.
 */
struct inode* create_dir( struct inode* parent, const char* name );

/* Check all the inodes that are directly referenced by
 * the node parent. If one of them has the name "name",
 * its inode pointer is returned.
 * parent must be directory.
 */
struct inode* find_inode_by_name( struct inode* parent, const char* name );

/* Delete the file given by its inode, if it is an inode
 * directly referenced by parent.
 * The function calls free_block for every block that is
 * referenced by this file. This removes those blocks from
 * simulate disk.
 * Returns 0 on success and -1 on error
 */
int delete_file( struct inode* parent, struct inode* node );

/* Delete the directory given by its inode, if it is an inode
 * directly referenced by parent.
 * The function fails if the node is still referencing other
 * inodes.
 * Returns 0 on success and -1 on error
 */
int delete_dir( struct inode* parent, struct inode* node );

/* Write the given inode root and all inodes referenced by it
 * to the master file table, following the oblig instructions.
 * No inodes are changed.
 */
void save_inodes( const char* master_file_table, struct inode* root );

/* Read the file master file table and create an inode in memory
 * for every inode that is stored in the file. Set the pointers
 * between inodes correctly.
 * The file containing the master file table remains unchanged.
 */
struct inode* load_inodes( const char* master_file_table );

/* This function releases all dynamically allocated memory
 * recursively for all the referenced inodes are visited
 * and their memory is released, finally also for the
 * node that is passed as a parameter.
 * The simulated disk and the file containing the master file
 * table are not changed.
 *
 * This function can be used to end a program after
 * save_inodes and helps you to avoid valgrind errors.
 */
void fs_shutdown( struct inode* node );

/* This function is handed out.
 *
 * It prints the node that is receives as a parameter,
 * and recurivesly all inodes that are stored below it.
 */
void debug_fs( struct inode* node );

/*******************************************************************************
 * BEGIN: ADD YOUR OWN FUNCTION DECLARATIONS BELOW HERE
 ******************************************************************************/

/*******************************************************************************
 * END: ADD YOUR OWN FUNCTION DECLARATIONS ABOVE HERE
 ******************************************************************************/

#endif

