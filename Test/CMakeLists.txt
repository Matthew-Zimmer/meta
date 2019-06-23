Target_Include_Directories(Meta PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../Include>)

Target_Compile_Features(Meta PUBLIC cxx_std_17)