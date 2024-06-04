cmake_minimum_required(VERSION 3.8)

PROJECT(dmgmock)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

LibImport("dmgmock" "src/dmgmock")

if(PROJECT_IS_TOP_LEVEL)
    ExeImport("test" "dmgmock;dmgmock_main")
endif(PROJECT_IS_TOP_LEVEL)

#ADD_CUSTOM_COMMAND(
#TARGET dmgmocktest
#PRE_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different
#${CMAKE_CURRENT_SOURCE_DIR}/config/test.csv
#$<TARGET_FILE_DIR:dmgmocktest>
#)

#INSTALL(DIRECTORY include/ DESTINATION include)
#INSTALL(TARGETS dmgmock
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)

#SET(DMGMOCK_LIBS "dmgmock")
#MARK_AS_ADVANCED(${DMGMOCK_LIBS})
