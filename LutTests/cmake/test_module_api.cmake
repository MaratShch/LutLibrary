cmake_minimum_required (VERSION 3.25)


function (lutlib_test name source libraries)
 
 set (GTest_NAME ${name}.gtest)
 set (GTest_PrivateLibraries gtest gtest_main)

 add_executable (
	${GTest_NAME}
        ${source}
 )
 
 target_include_directories (
	${GTest_NAME}
	BEFORE PRIVATE ${GTest_PrivateIncludes}
	BEFORE PUBLIC  ${GTest_Includes}
 )

 target_link_libraries (
	${GTest_NAME}
    PRIVATE ${GTest_PrivateLibraries}
    PUBLIC  ${libraries}
 )

 install (TARGETS ${GTest_NAME} DESTINATION ${CMAKE_INSTALL_TST_DIRECTORY})
 
endfunction(lutlib_test)
