#include "inode.h"
#include "block_allocation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>


/* Free a single inode structure and its allocated fields. */
static void free_inode(struct inode* node) {
    if (!node) return;
    free(node->name);
    if (node->entries) free(node->entries);
    free(node);
}

/* Validate a filename or directory name. Returns 1 if acceptable. */
static int is_valid_name(const char* name) {
    if (name == NULL || *name == '\0') return 0;
    if (strcmp(name,"/") == 0) return 1;
    for (const char* i = name; *i; i++) {
        if (!isalnum((unsigned char)*i) && *i != '.' && *i != '_') {
            return 0;
        }
    }
    return 1;
}

static uint32_t next_inode_id = 0;
/* Allocate and initialize a basic inode object (directory or file). */
static struct inode* create_inode(const char* name, char is_directory, char is_readonly) {
    if (is_valid_name(name) == 0) {
        return NULL;
    }

    struct inode* node = (struct inode*)malloc(sizeof(struct inode));
    if(node == NULL) {
        perror("Failed to allocate memory for inode");
        return NULL;
    }

    node->name = strdup(name);
    if (node->name == NULL) {
        perror("Failed to allocate memory for name");
        free(node);
        return NULL;
    }

    node->id = next_inode_id++;
    node->is_directory = is_directory;
    node->is_readonly = is_readonly;
    node->filesize = 0;
    node->num_entries = 0;
    node->entries = NULL;
    return node;
}

/* Free blocks referenced by the given extents array by calling free_block. */
static void free_allocated_extents(struct Extent* extents, uint32_t count) { 
    if (!extents) return;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t start = extents[i].blockno;
        uint32_t length = extents[i].extent;
        for (uint32_t j = 0; j < length; j++) {
            free_block(start + j);
        }
    }
}

/* Allocate extents for a file to cover `size_in_bytes`. Uses max extent=4.
 * On success the file->entries and file->num_entries are populated.
 */
static int allocate_blocks_for_file(struct inode* file, uint32_t size_in_bytes) {
    if (file == NULL || file->is_directory) {
        return -1;
    }

    uint32_t num_blocks = (size_in_bytes + BLOCKSIZE - 1) / BLOCKSIZE;
    if (num_blocks == 0) {
        return 0;  
    }

    uint32_t max_extent_size = 4;
    uint32_t num_extents = (num_blocks + max_extent_size - 1) / max_extent_size;

    file->entries = (uintptr_t*)calloc(num_extents, sizeof(struct Extent));
    if (!file->entries) {
        perror("Failed to allocate entries");
        return -1;
    }

    struct Extent* extents = (struct Extent*)file->entries;
    uint32_t remaining_blocks = num_blocks;

    for (uint32_t i = 0; i < num_extents; i++) {
        uint32_t extent_size = (remaining_blocks > max_extent_size) ? max_extent_size : remaining_blocks;

        if (extent_size == 0) {
            fprintf(stderr, "Error: Attempting to allocate zero-length extent\n");
            free_allocated_extents(extents, i);
            free(file->entries);
            file->entries = NULL;
            return -1;
        }

        int blockno = allocate_block(extent_size);
        if (blockno == -1) {
            free_allocated_extents(extents, i);
            free(file->entries);
            file->entries = NULL;
            return -1;
        }

        extents[i].blockno = blockno;
        extents[i].extent = extent_size;

        remaining_blocks -= extent_size;
    }

    file->num_entries = num_extents;
    file->filesize = size_in_bytes;
    return 0;
}

/* Create a file inode, allocate its blocks, and link it into `parent`.
 * Returns the new inode or NULL on failure.
 */
struct inode* create_file(struct inode* parent, const char* name, char readonly, int size_in_bytes) {
    if (parent != NULL && (*parent).is_directory == 0) { 
        return NULL;
    }

    if (find_inode_by_name(parent,name) != NULL) { 
        return NULL;
    }

    struct inode* file = create_inode(name, 0, readonly); 
    if (file == NULL) {
        return NULL;
    }
    
    uint32_t block_allocation = allocate_blocks_for_file(file, size_in_bytes);
    if (block_allocation != 0) {
        free(file->name);
        free(file->entries);
        free(file);
        return NULL;
    }

    if (parent != NULL) {
        uintptr_t* new_entries = realloc(parent->entries, (parent->num_entries + 1) * sizeof(struct inode*));
        if (new_entries == NULL) {
            perror("Failed to allocate memory for parent entries");
            free(file->name);
            free(file->entries);
            free(file);
            return NULL;
        }
        parent->entries = new_entries;
        parent->entries[parent->num_entries] = (uintptr_t)file;
        parent->num_entries++;
    }

    return file;
}

/* Create a directory inode and add it to the parent directory. */
struct inode* create_dir(struct inode* parent, const char* name) {
    if (!parent && strcmp(name, "/") == 0) {
        struct inode* root = create_inode(name, 1, 0);
        return root;
    }

    if (!parent || !parent->is_directory) {
        return NULL;
    }

    struct inode* existing = find_inode_by_name(parent, name);
    if (existing) {
        return NULL;
    }

    struct inode* dir = create_inode(name, 1, 0);
    if (!dir) {
        return NULL;
    }

    uintptr_t* new_entries = (uintptr_t*)realloc(parent->entries, (parent->num_entries + 1) * sizeof(uintptr_t));
    if (!new_entries) {
        perror("Failed to reallocate memory");
        free(dir->name);
        free(dir->entries);
        free(dir);
        return NULL;
    }

    parent->entries = new_entries;
    parent->entries[parent->num_entries] = (uintptr_t)dir;
    parent->num_entries++;

    return dir;
}

/* Find a direct child of `parent` with matching `name`. */
struct inode* find_inode_by_name(struct inode* parent, const char* name) {
    if (parent == NULL || !parent->is_directory || name == NULL) {
        return NULL;
    }

    for (uint32_t i = 0; i < parent->num_entries; i++) {
        struct inode* child = (struct inode*)parent->entries[i];
        if (strcmp(child->name, name) == 0) {
            return child;
        }
    }
    
    return NULL;
}

/* Remove a file inode from `parent`, free its extents and inode memory. */
int delete_file(struct inode* parent, struct inode* node) {
    if (!parent || !parent->is_directory || !node || node->is_directory) {
        return -1; 
    }
    
    uint32_t i = 0; 
    for (; i < parent->num_entries; i++) {
        struct inode* child = (struct inode*)parent->entries[i];
        if (child == node) {
            break;
        }
    }
    
    if (i == parent->num_entries) {
        return -1; 
    }

    free_allocated_extents((struct Extent*)node->entries, node->num_entries);

    memmove(&parent->entries[i], &parent->entries[i + 1],
            (parent->num_entries - i - 1) * sizeof(uintptr_t));
    parent->num_entries--;
    
    if (parent->num_entries > 0) {
        uintptr_t* new_entries = realloc(parent->entries, parent->num_entries * sizeof(uintptr_t));
        if (new_entries) {
            parent->entries = new_entries;
        }
    } else {
        free(parent->entries);
        parent->entries = NULL;
    }

    free_inode(node);
    return 0; 
}

/* Remove an empty directory inode from `parent` and free it. */
int delete_dir(struct inode* parent, struct inode* node) {
    if (!parent || !parent->is_directory || !node || !node->is_directory) {
        return -1; 
    }

    if (node->num_entries > 0) {
        return -1; 
    }

    uint32_t i = 0;
    for (; i < parent->num_entries; i++) {
        struct inode* child = (struct inode*)parent->entries[i];
        if (child == node) {
            break;
        }
    }

    if (i == parent->num_entries) {
        return -1; 
    }

    memmove(&parent->entries[i], &parent->entries[i + 1],
            (parent->num_entries - i - 1) * sizeof(uintptr_t));
    parent->num_entries--;

    if (parent->num_entries > 0) {
        uintptr_t* new_entries = realloc(parent->entries, parent->num_entries * sizeof(uintptr_t));
        if (new_entries) {
            parent->entries = new_entries;
        }
    } else {
        free(parent->entries);
        parent->entries = NULL;
    }

    free_inode(node);
    return 0; 
}

/* Recursively write inode entries to the master file table in a simple text format. */
static void save_inode_recursive(FILE* file, struct inode* node, struct inode* parent) {
    if (node == NULL) {
        return;
    }

    fprintf(file, "ID:%u\n", node->id);
    fprintf(file, "NAME:%s\n", node->name);
    fprintf(file, "IS_DIR:%d\n", node->is_directory);
    fprintf(file, "READ_ONLY:%d\n", node->is_readonly);
    fprintf(file, "SIZE:%u\n", node->filesize);
    
    if (parent != NULL) {
        fprintf(file, "PARENT:%u\n", parent->id);
    } else {
        fprintf(file, "PARENT:NULL\n");
    }

    if (!node->is_directory) {
        fprintf(file, "NUM_ENTRIES:%u\n", node->num_entries);
        struct Extent* extents = (struct Extent*)node->entries;
        for (uint32_t i = 0; i < node->num_entries; i++) {
            fprintf(file, "EXTENT:%u,%u\n", extents[i].blockno, extents[i].extent);
        }
    }
    
    fprintf(file, "END_NODE\n\n");

    if (node->is_directory) {
        for (uint32_t i = 0; i < node->num_entries; i++) {
            struct inode* child = (struct inode*)node->entries[i];
            save_inode_recursive(file, child, node);
        }
    }
}

/* Write the entire filesystem rooted at `root` to `master_file_table`. */
void save_inodes(const char* master_file_table, struct inode* root) {
    FILE* file = fopen(master_file_table, "w");
    if (file == NULL) {
        perror("Error in open master file table for writing");
        return;
    }

    fprintf(file, "MASTER FILE TABLE\n");
    save_inode_recursive(file, root, NULL);
    fclose(file);
}

/* Load inodes from a master file table and reconstruct the in-memory tree.
 * High level steps:
 *  - Reset/format block allocation table.
 *  - Open the binary master file table and read a sequence of records.
 *    Each record encodes: ID, name length + name, flags, filesize (for files),
 *    number of entries and then either child IDs (for directories) or
 *    extents (for files).
 *  - For files, allocate new blocks according to the saved filesize and
 *    copy or discard the on-disk extent list as appropriate.
 *  - Build an ID->node map while reading, then perform a second pass to
 *    convert directory child ID lists into pointers to the in-memory nodes.
 */
struct inode* load_inodes(const char* master_file_table) {
    format_disk();

    FILE* file = fopen(master_file_table, "rb");
    if (file == NULL) {
        perror("Error in open master file table for reading");
        return NULL;
    }

    /* Temporary mapping from ID -> allocated node used for the two-pass load */
    struct {
        uint32_t id;
        struct inode* node;
    } *id_map = NULL;
    uint32_t id_map_size = 0;

    next_inode_id = 0;
    struct inode* root = NULL;

    /* First pass: read all node records and create in-memory nodes. */
    while (1) {
        uint32_t id;
        size_t read = fread(&id, sizeof(uint32_t), 1, file);
        if (read != 1) {
            if (feof(file)) break; /* Normal EOF */
            perror("Error reading ID from master file table");
            goto cleanup;
        }

        if (id >= next_inode_id) next_inode_id = id + 1;

        /* Read name length and name string */
        uint32_t name_length;
        if (fread(&name_length, sizeof(uint32_t), 1, file) != 1) {
            perror("Error reading name length from master file table");
            goto cleanup;
        }

        if (name_length == 0 || name_length > 255) {
            fprintf(stderr, "Invalid name length: %u\n", name_length);
            goto cleanup;
        }

        char* name = malloc(name_length + 1);
        if (name == NULL) {
            perror("Error in allocate memory for name");
            goto cleanup;
        }

        if (fread(name, 1, name_length, file) != name_length) {
            perror("Error reading name from MFT");
            free(name);
            goto cleanup;
        }
        name[name_length] = '\0'; 

        /* Flags: directory and readonly bytes */
        char is_directory, is_readonly;
        if (fread(&is_directory, 1, 1, file) != 1 ||
            fread(&is_readonly, 1, 1, file) != 1) {
            perror("Error reading flags from MFT");
            free(name);
            goto cleanup;
        }

        /* Allocate a fresh node structure and populate basic fields */
        struct inode* node = malloc(sizeof(struct inode));
        if (node == NULL) {
            perror("Error in allocate memory for inode");
            free(name);
            goto cleanup;
        }

        node->id = id;
        node->name = name;
        node->is_directory = is_directory;
        node->is_readonly = is_readonly;
        node->entries = NULL;
        node->num_entries = 0;

        /* Files store a filesize; directories do not. */
        if (!is_directory) {
            if (fread(&node->filesize, sizeof(uint32_t), 1, file) != 1) {
                perror("Error reading filesize from MFT");
                free(name);
                free(node);
                goto cleanup;
            }
        } else {
            node->filesize = 0;
        }

        /* Read number of entries: for directories this is child ID list length,
         * for files this is the number of extents recorded on disk. */
        if (fread(&node->num_entries, sizeof(uint32_t), 1, file) != 1) {
            perror("Error reading num_entries from MFT");
            free(name);
            free(node);
            goto cleanup;
        }

        if (node->num_entries > 0) {
            if (is_directory) {
                /* Read array of child IDs (uint64_t used on-disk to be portable) */
                node->entries = calloc(node->num_entries, sizeof(uint64_t));
                if (node->entries == NULL) {
                    perror("Error in allocate memory for directory entries");
                    free(name);
                    free(node);
                    goto cleanup;
                }

                if (fread(node->entries, sizeof(uint64_t), node->num_entries, file) != node->num_entries) {
                    perror("Error reading directory entries from MFT");
                    free(node->entries);
                    free(name);
                    free(node);
                    goto cleanup;
                }
            } else {
                /* Read on-disk extent list for files (blockno, extent) pairs */
                node->entries = calloc(node->num_entries, sizeof(struct Extent));
                if (node->entries == NULL) {
                    perror("Error in allocate memory for file extents");
                    free(name);
                    free(node);
                    goto cleanup;
                }

                if (fread(node->entries, sizeof(struct Extent), node->num_entries, file) != node->num_entries) {
                    perror("Error reading file extents from MFT");
                    free(node->entries);
                    free(name);
                    free(node);
                    goto cleanup;
                }
            }
        }

        /* For files: allocate fresh blocks according to filesize. The saved
         * on-disk extents are read into `original_entries` above; after
         * allocation we free the on-disk list if the runtime representation
         * changed. This keeps runtime extents consistent with the BAT.
         */
        if (!is_directory) {
            struct Extent* original_entries = (struct Extent*)node->entries;
            if (allocate_blocks_for_file(node, node->filesize) != 0) {
                fprintf(stderr, "Error: Failed to allocate blocks for file\n");
                free(node->entries);
                free(name);
                free(node);
                goto cleanup;
            }
            
            if (original_entries != (struct Extent*)node->entries) {
                free(original_entries);
            }
        }

        /* Record the node in the temporary ID map for the second pass */
        id_map = realloc(id_map, (id_map_size + 1) * sizeof(*id_map));
        if (id_map == NULL) {
            perror("Failed to allocate memory for ID map");
            if (node->entries) free(node->entries);
            free(name);
            free(node);
            goto cleanup;
        }

        id_map[id_map_size].id = id;
        id_map[id_map_size].node = node;
        id_map_size++;

        if (strcmp(name, "/") == 0 && is_directory) {
            root = node;
        }
    }

    /* Second pass: convert stored child ID arrays into pointer arrays for
     * directory entries. This resolves references between nodes using the
     * temporary `id_map` built during the first pass. */
    for (uint32_t i = 0; i < id_map_size; i++) {
        struct inode* node = id_map[i].node;

        if (node->is_directory && node->num_entries > 0) {
            uint64_t* id_entries = (uint64_t*)node->entries;
            struct inode** ptr_entries = malloc(node->num_entries * sizeof(struct inode*));
            if (ptr_entries == NULL) {
                perror("Failed to allocate memory for pointer entries");
                goto cleanup;
            }

            for (uint32_t j = 0; j < node->num_entries; j++) {
                uint32_t child_id = (uint32_t)id_entries[j];

                struct inode* child = NULL;
                for (uint32_t k = 0; k < id_map_size; k++) {
                    if (id_map[k].id == child_id) {
                        child = id_map[k].node;
                        break;
                    }
                }

                if (child == NULL) {
                    fprintf(stderr, "Invalid child ID %llu\n", (unsigned long long)child_id);
                    free(ptr_entries);
                    goto cleanup;
                }

                ptr_entries[j] = child;
            }

            free(id_entries);
            node->entries = (uintptr_t*)ptr_entries;
        }
    }

    /* Cleanup and return the reconstructed root inode */
    fclose(file);
    free(id_map);
    return root;

cleanup:
    /* On error: free any partially built nodes and close resources. */
    if (id_map != NULL) {
        for (uint32_t i = 0; i < id_map_size; i++) {
            struct inode* node = id_map[i].node;
            if (node) {
                free_inode(node);
            }
        }
        free(id_map);
    }

    if (file != NULL) {
        fclose(file);
    }

    return NULL;
}

/* Recursively free the entire inode tree and any allocated extents. */
void fs_shutdown(struct inode* inode) {
    if (inode == NULL) {
        return;
    }

    if (inode->is_directory) {
        for (uint32_t i = 0; i < inode->num_entries; i++) {
            struct inode* child = (struct inode*)inode->entries[i];
            fs_shutdown(child);
        }
    } else {
        free_allocated_extents((struct Extent*)inode->entries, inode->num_entries);
    }

    free_inode(inode);
}

static int indent = 0;

/* Helpers used by debug_fs to collect and print block usage. */
static void debug_fs_print_table(const char* table);
static void debug_fs_tree_walk(struct inode* node, char* table);

void debug_fs(struct inode* node) {
    char* table = calloc(NUM_BLOCKS, 1);
    debug_fs_tree_walk(node, table);
    debug_fs_print_table(table);
    free(table);
}

static void debug_fs_tree_walk(struct inode* node, char* table) {
    if(node == NULL) return;
    for(int i=0; i<indent; i++)
        printf("  ");
    if(node->is_directory) {
        printf("%s (id %d)\n", node->name, node->id);
        indent++;
        for(int i=0; i<node->num_entries; i++) {
            struct inode* child = (struct inode*)node->entries[i];
            debug_fs_tree_walk(child, table);
        }
        indent--;
    } else {
        printf("%s (id %d size %d)\n", node->name, node->id, node->filesize);
        uint32_t* extents = (uint32_t*)node->entries;
        for(int i=0; i<node->num_entries; i++) {
            for(int j=0; j<extents[2*i+1]; j++) {
                table[extents[2*i]+j] = 1;
            }
        }
    }
}

static void debug_fs_print_table(const char* table) {
    printf("Blocks recorded in master file table:");
    for(int i=0; i<NUM_BLOCKS; i++) {
        if(i % 20 == 0) printf("\n%03d: ", i);
        printf("%d", table[i]);
    }
    printf("\n\n");
}


static void print_blocks(void) {
    debug_disk();
}