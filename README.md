# Blockbased Filesystem 

This project implements a small, educational inode-based filesystem in C. The goal is to demonstrate how files and directories can be represented in memory, 
how disk blocks can be allocated efficiently, and how filesystem state can be saved and restored.

The filesystem uses in-memory inode structures for both files and directories, along with an extent-based block allocation scheme. 
Each file is stored as a list of extents (contiguous ranges of blocks) together with its file size. Block allocation is handled in block_allocation.c 
through allocate_block and free_block, using a global block table, while inode.c is responsible for creating and deleting inodes, managing directory entries, 
and freeing file extents when files are removed.

Persistence is implemented using a simple plain-text master file table. The filesystem state is written to disk by save_inodes, and restored by load_inodes, 
which parses the file, rebuilds the internal inode map, reconnects directory relationships, and re-allocates blocks for each file so that the metadata is correctly 
reconstructed in memory.

The project is built using CMake and includes several small test programs (create_fs_*, load_fs_*, check_fs, and check_disk) that exercise block allocation, 
saving and loading, and deletion. Test outputs are written to the test-outputs directory and are meant to be compared with the provided expected-outputs.

For full verification of memory correctness, the tests can be run under Valgrind (the expected outputs assume Valgrind logs). 
Functionally, however, the project demonstrates correct block allocation, persistence, and cleanup for the provided example scenarios.
