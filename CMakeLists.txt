cmake_minimum_required(VERSION 3.14)

#
# This tells CMake that your project's name is homeexam-01, and that the programs
# here are written in C.
# 
project(Oblig02
	VERSION 1.0
	DESCRIPTION "CMake configuration file for the second home exam IN2140 v25"
        LANGUAGES C)

#
# This tells CMake that the compilation should use the version of the C standard
# from 2011.
# This has no effect on Windows.
#
set(CMAKE_C_STANDARD 11)

#
# This tells CMake to create compilation rules that add debug information. When
# your program is built successfully, you will be able to search for bugs with
# gdb and valgrind. The build type Release would make a program that cannot be
# debugged with these tools.
#
set(CMAKE_BUILD_TYPE Debug)

# The compile flag -pg can be added to compilation and linking if you want to use
# the gprof tool on Linux.
# add_compile_options(-pg)
# add_link_options(-pg)

#
# Include the top source directory in the search path for include files.
#
include_directories(${CMAKE_SOURCE_DIR})

#
# This tells CMake to create rules for making an executable program named homeexam-01
# from the source files tests.c the_apple.c and the_apple.h
#
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

#
# This creates a make rule that helps you create your delivery.
# You call it with "make package_source"
# This will generate the archive file HomeExam01-1.0-Source.zip,
# which you deliver.
#
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

