CMake_Minimum_Required(VERSION 3.13)

Project(Meta)

If(DEFINED TESTING)
	If(${TESTING} STREQUAL true)
		Add_Executable(Meta "Test/Main.cpp")
		Target_Compile_Definitions(Meta PRIVATE TESTING)
		Add_Subdirectory(Test)
	Else()
		Add_Library(Meta INTERFACE)
	EndIf()
	Unset(TESTING CACHE)
ELSE()
	Add_Library(Meta INTERFACE)
EndIf()

Add_Subdirectory(Source)
Add_Subdirectory(Include)
Add_Subdirectory(Imp)

Target_Compile_Features(Meta 
    INTERFACE cxx_std_17)

Set(CMAKE_CXX_STANDARD "libc++")
#Set(CMAKE_CXX_FLAGS "-Wall" "-Wextra")

Install(TARGETS Meta EXPORT Meta DESTINATION lib/Meta)
Install(EXPORT Meta DESTINATION lib/Meta)
Install(FILES MetaConfig.cmake DESTINATION lib/Meta)