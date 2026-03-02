
#ifndef INODE_H
#define INODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Represents an allocated extent: starting block and length in blocks. */
struct Extent {
	uint32_t blockno;
	uint32_t extent;
};

/* In-memory inode structure for files and directories.
 * `entries` is an array of pointers for directories or extents for files.
 */
struct inode {
	uint32_t   id;
	char*      name;
	char       is_directory;
	char       is_readonly;
	uint32_t   filesize;
	uint32_t   num_entries;
	uintptr_t* entries;
};

/* Create a file under `parent` with the given name, readonly flag and size.
 * Allocates blocks for the file and returns the new inode or NULL on error.
 */
struct inode* create_file(struct inode* parent,
						  const char* name,
						  char readonly,
						  int size_in_bytes);

/* Create a directory under `parent` with the given name. */
struct inode* create_dir(struct inode* parent, const char* name);

/* Lookup a direct child inode by name in `parent`; returns NULL if not found. */
struct inode* find_inode_by_name(struct inode* parent, const char* name);

/* Delete a file (or directory) entry from `parent`. Returns 0 on success. */
int delete_file(struct inode* parent, struct inode* node);
int delete_dir(struct inode* parent, struct inode* node);

/* Persist the in-memory inode tree to `master_file_table`. */
void save_inodes(const char* master_file_table, struct inode* root);

/* Load inodes from `master_file_table` and reconstruct pointers. */
struct inode* load_inodes(const char* master_file_table);

/* Recursively free all inodes and their resources. */
void fs_shutdown(struct inode* node);

/* Pretty-print the inode tree to stdout for debugging. */
void debug_fs(struct inode* node);

#endif

