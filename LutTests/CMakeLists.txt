cmake_minimum_required (VERSION 3.25)

message (STATUS "Include LutTests repository")

set (LUT_TESTS_FILES_FOLDER	${CMAKE_CURRENT_SOURCE_DIR})

# IRIDAS files for tests (CUBE 1D and CUBE 3D format)
set (CUBE_FILES_FOLDER  	${LUT_TESTS_FILES_FOLDER}/Cube)
set (CUBE_1D_FILES_FOLDER  	${CUBE_FILES_FOLDER}/1D)
set (CUBE_3D_FILES_FOLDER  	${CUBE_FILES_FOLDER}/3D)

# Common LUT Format files for tests (CLF files)
set (CLF_FILES_FOLDER		${LUT_TESTS_FILES_FOLDER}/CLF)

# CCF LUT format (CSF files)
set (CSP_FILES_FOLDER		${LUT_TESTS_FILES_FOLDER}/CSP)

#destination directory for install LUT files for debug and test purposes
# should be defined into cmake file for target/project configuration
install (DIRECTORY ${CUBE_3D_FILES_FOLDER} DESTINATION ${CMAKE_INSTALL_LUT_TST_DIRECTORY}/CUBE)
install (DIRECTORY ${CUBE_1D_FILES_FOLDER} DESTINATION ${CMAKE_INSTALL_LUT_TST_DIRECTORY}/CUBE)
install (DIRECTORY ${CLF_FILES_FOLDER}     DESTINATION ${CMAKE_INSTALL_LUT_TST_DIRECTORY})
install (DIRECTORY ${CSP_FILES_FOLDER}     DESTINATION ${CMAKE_INSTALL_LUT_TST_DIRECTORY})
