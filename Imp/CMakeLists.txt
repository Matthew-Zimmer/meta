Target_Include_Directories(Meta INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/Meta>)
Set(Imps 
	)

Install(FILES ${Imps} DESTINATION include/Meta)