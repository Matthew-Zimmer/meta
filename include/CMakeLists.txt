target_include_directories(meta INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/meta>)

target_include_directories(meta_test PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

set(headers 
	meta.hpp)

install(FILES ${headers} DESTINATION include/meta)