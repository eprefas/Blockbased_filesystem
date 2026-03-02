cmake_minimum_required(VERSION 3.14)


project(Oblig02
	VERSION 1.0
	DESCRIPTION "CMake configuration file for the second home exam IN2140 v25"
        LANGUAGES C)


set(CMAKE_C_STANDARD 11)


set(CMAKE_BUILD_TYPE Debug)


include_directories(${CMAKE_SOURCE_DIR})


add_executable(	check_disk
		check_disk.c
		block_allocation.c block_allocation.h )

add_executable(	check_fs
		check_fs.c
		inode.c inode.h
		../block_allocation.c ../block_allocation.h )

add_executable(	load_fs_1
		load_fs_1.c
		block_allocation.c block_allocation.h
                inode.c inode.h )

add_executable(	load_fs_2
		load_fs_2.c
		block_allocation.c block_allocation.h
                inode.c inode.h )

add_executable(	load_fs_3
		load_fs_3.c
		block_allocation.c block_allocation.h
                inode.c inode.h )

add_executable(	create_fs_1
		create_fs_1.c
		block_allocation.c block_allocation.h
		inode.c inode.h )

add_executable(	create_fs_2
		create_fs_2.c
		block_allocation.c block_allocation.h
		inode.c inode.h )

add_executable(	create_fs_3
		create_fs_3.c
		block_allocation.c block_allocation.h
		inode.c inode.h )

add_executable(	create_and_delete
		create_and_delete.c
		block_allocation.c block_allocation.h
		inode.c inode.h )

add_subdirectory( test-cases )


set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_IGNORE_FILES
	/\.git/
	/build/
	".*Makefile"
	/CMakeFiles/
	".*CMakeCache.txt"
	".*CPack.*"
	".*cmake"
	".*.zip"
	".*~$" )
include(CPack)

