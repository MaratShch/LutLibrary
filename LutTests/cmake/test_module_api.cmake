cmake_minimum_required (VERSION 3.25)


function (lutlib_test name source libraries)

  set (GTest_NAME ${name}.gtest)
  set (GTest_PrivateLibraries gtest gtest_main)

  # Create list of libraries
  set(library_list "${libraries}")

  # If additional libraries are passed after the known arguments, add them to list
  set(num_fixed_args 4) # function, name, source, libraries
  set(num_passed_args ${ARGC})
  math(EXPR num_extra_libs "${num_passed_args} - ${num_fixed_args}")

  if(${num_extra_libs} GREATER 0)
    message(STATUS "Additional libs to be added to library_list: ${num_extra_libs}")
    foreach(i RANGE ${num_extra_libs})
      math(EXPR arg_index "${i} + ${num_fixed_args}")
      list(APPEND library_list "${ARGV${arg_index}}")
    endforeach()
  endif()

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
        PUBLIC  ${library_list}  # Link the libraries (single or multiple)
  )

  if (TST_PRIVATE_COMPILATION_DEFINES)
 	target_compile_definitions (${GTest_NAME} PRIVATE ${TST_PRIVATE_COMPILATION_DEFINES})
	unset(TST_PRIVATE_COMPILATION_DEFINES)
  endif(TST_PRIVATE_COMPILATION_DEFINES)

  install (TARGETS ${GTest_NAME} DESTINATION ${CMAKE_INSTALL_TST_DIRECTORY})

endfunction(lutlib_test)




