project (rapidxml LANGUAGES CXX)

set(LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR})

set(RapidXml_LicenseFile ${LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES}/license.txt)

set(RapidXml_Library_SOURCES
    ${LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES}/rapidxml.hpp
    ${LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES}/rapidxml_iterators.hpp
    ${LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES}/rapidxml_print.hpp
    ${LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES}/rapidxml_utils.hpp)

add_library (${PROJECT_NAME} INTERFACE)
target_include_directories (${PROJECT_NAME} INTERFACE 
    $<BUILD_INTERFACE:${LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES}>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)
target_sources (${PROJECT_NAME}
        INTERFACE FILE_SET HEADERS
        BASE_DIRS ${LutLibrary_3RD_PARTY_RAPIDXML_ROOT_SOURCES}
        FILES ${RapidXml_Library_SOURCES}
)

install (FILES ${RapidXml_Library_SOURCES}
         DESTINATION ${CMAKE_INSTALL_HXX_DIRECTORY}/${PROJECT_NAME})
install (FILES ${RapidXml_LicenseFile}
         DESTINATION ${CMAKE_INSTALL_HXX_DIRECTORY}/${PROJECT_NAME}/license)  
