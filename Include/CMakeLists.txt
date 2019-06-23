Target_Include_Directories(Meta INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/Meta>)

Set(Headers 
	Meta.hpp)

Install(FILES ${Headers} DESTINATION include/Meta)