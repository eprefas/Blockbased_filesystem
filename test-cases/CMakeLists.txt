#
# The following is a number of custom tests that you may execute. You can run them
# by calling "make tests" (all of them), or invididual tests, like "make check_disk_test_1".
# This is only for your convenience, and is not required.
#

find_program(HAVE_VALGRIND valgrind)

if(HAVE_VALGRIND)
add_custom_command( OUTPUT check_disk_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/check_disk"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-check_disk"
  	            DEPENDS make_test_out check_disk )

add_custom_command( OUTPUT check_fs_test1
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/check_fs"
		         "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-1"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-1"
  	            DEPENDS check_fs )

add_custom_command( OUTPUT check_fs_test2
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/check_fs"
		         "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-2"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-2"
  	            DEPENDS check_fs )

add_custom_command( OUTPUT check_fs_test3
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/check_fs"
		         "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-3"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-3"
  	            DEPENDS check_fs )

add_custom_command( OUTPUT load_fs_1_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/load_fs_1"
		         "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-1"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-1"
  	            DEPENDS load_fs_1 )

add_custom_command( OUTPUT load_fs_2_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/load_fs_2"
	                 "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-2"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-2"
  	            DEPENDS load_fs_2 )

add_custom_command( OUTPUT load_fs_3_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/load_fs_3"
	                 "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-3"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-3"
  	            DEPENDS load_fs_3 )

add_custom_command( OUTPUT create_fs_1_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/create_fs_1"
		         "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_fs_1"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_fs_1"
  	            DEPENDS make_test_out create_fs_1 )

add_custom_command( OUTPUT create_fs_2_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/create_fs_2"
		         "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_fs_2"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_fs_2"
  	            DEPENDS make_test_out create_fs_2 )

add_custom_command( OUTPUT create_fs_3_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/create_fs_3"
		         "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_fs_3"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_fs_3"
  	            DEPENDS make_test_out create_fs_3 )

add_custom_command( OUTPUT create_and_delete_test
  	            COMMAND valgrind --track-origins=yes --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all
	            ARGS "${PROJECT_BINARY_DIR}/create_and_delete"
		         "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_and_delete"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_and_delete"
  	            DEPENDS make_test_out create_and_delete )
else()
add_custom_command( OUTPUT check_disk_test
  	            COMMAND check_disk
		    ARGS "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-check_disk"
  	            DEPENDS make_test_out check_disk )

add_custom_command( OUTPUT check_fs_test1
  	            COMMAND check_fs
	            ARGS "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-1"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-1"
  	            DEPENDS check_fs )

add_custom_command( OUTPUT check_fs_test2
  	            COMMAND check_fs
	            ARGS "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-2"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-2"
  	            DEPENDS check_fs )

add_custom_command( OUTPUT check_fs_test3
  	            COMMAND check_fs
	            ARGS "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-3"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-3"
  	            DEPENDS check_fs )

add_custom_command( OUTPUT load_fs_1_test
  	            COMMAND load_fs_1
	            ARGS "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-1"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-1"
  	            DEPENDS load_fs_1 )

add_custom_command( OUTPUT load_fs_2_test
  	            COMMAND load_fs_2
	            ARGS "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-2"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-2"
  	            DEPENDS load_fs_2 )

add_custom_command( OUTPUT load_fs_3_test
  	            COMMAND load_fs_3
	            ARGS "${PROJECT_SOURCE_DIR}/test-inputs/master_file_table-load-example-3"
		         "${PROJECT_SOURCE_DIR}/test-inputs/block_allocation_table-load-example-3"
  	            DEPENDS load_fs_3 )

add_custom_command( OUTPUT create_fs_1_test
  	            COMMAND create_fs_1
	            ARGS "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_fs_1"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_fs_1"
  	            DEPENDS make_test_out create_fs_1 )

add_custom_command( OUTPUT create_fs_2_test
  	            COMMAND create_fs_2
	            ARGS "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_fs_2"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_fs_2"
  	            DEPENDS make_test_out create_fs_2 )

add_custom_command( OUTPUT create_fs_3_test
  	            COMMAND create_fs_3
	            ARGS "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_fs_3"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_fs_3"
  	            DEPENDS make_test_out create_fs_3 )

add_custom_command( OUTPUT create_and_delete_test
  	            COMMAND create_and_delete
	            ARGS "${PROJECT_SOURCE_DIR}/test-outputs/master_file_table-create_and_delete"
		         "${PROJECT_SOURCE_DIR}/test-outputs/block_allocation_table-create_and_delete"
  	            DEPENDS make_test_out create_and_delete )
endif()

add_custom_command( OUTPUT make_test_out
		    COMMAND mkdir
		    ARGS "-p" "${PROJECT_SOURCE_DIR}/test-outputs" )

add_custom_target( tests
  	           DEPENDS check_disk_test
	                   check_fs_test1 check_fs_test2 check_fs_test3
		           load_fs_1_test load_fs_2_test load_fs_3_test
		           create_fs_1_test create_fs_2_test create_fs_3_test
		           create_and_delete_test )

add_custom_target( test-1-1 DEPENDS check_disk_test )
add_custom_target( test-2-1 DEPENDS check_fs_test1 )
add_custom_target( test-2-2 DEPENDS check_fs_test2 )
add_custom_target( test-2-3 DEPENDS check_fs_test3 )
add_custom_target( test-3-1 DEPENDS load_fs_1_test )
add_custom_target( test-3-2 DEPENDS load_fs_2_test )
add_custom_target( test-3-3 DEPENDS load_fs_3_test )
add_custom_target( test-4-1 DEPENDS create_fs_1_test )
add_custom_target( test-4-2 DEPENDS create_fs_2_test )
add_custom_target( test-4-3 DEPENDS create_fs_3_test )
add_custom_target( test-5-1 DEPENDS create_and_delete_test )

