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
    PRIVATE ${GTest_PrivateLibraries} ${TST_PRIVATE_LINK_LIBRARIES}
    PUBLIC  ${libraries}
 )

 if (TST_PRIVATE_COMPILATION_DEFINES)
 	target_compile_definitions (${GTest_NAME} PRIVATE ${TST_PRIVATE_COMPILATION_DEFINES})
	unset(TST_PRIVATE_COMPILATION_DEFINES)
 endif(TST_PRIVATE_COMPILATION_DEFINES)
 
 install (TARGETS ${GTest_NAME} DESTINATION ${CMAKE_INSTALL_TST_DIRECTORY})
 
endfunction(lutlib_test)
