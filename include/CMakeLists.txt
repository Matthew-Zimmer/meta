target_include_directories(meta INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/meta>)

set(headers 
	meta.hpp)

install(FILES ${headers} DESTINATION include/meta)