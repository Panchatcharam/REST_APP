cmake_minimum_required(VERSION 3.5)

project(RestApp)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/config)

include(BuildConfig)

set(APP_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)
set(LIB_INSTALL_PATH ${APP_INSTALL_PATH}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)

set(Libraries
   libcurl.so
   libssl.so
   libcrypto.so
   libRest.so
   )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/)

set(LibraryDirs
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/curl
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib
   ${CMAKE_CURRENT_SOURCE_DIR}/../lib/openssl
   )
set(ClInclude
   ${CMAKE_CURRENT_SOURCE_DIR}/../include/main.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../include/CommonDefs.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../include/RestAPIFacade.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../include/RestApp.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../include/RestAppWorker.h
   ${CMAKE_CURRENT_SOURCE_DIR}/../include/ThreadRunner.h
   )

set(ClCompile
   ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/RestApp.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/RestAppWorker.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/ThreadRunner.cpp)
   
find_package(Threads)

link_directories(${LibraryDirs})

# Create Executable
add_executable(RestApp ${ClInclude} ${ClCompile})

target_link_libraries(RestApp ${Libraries} ${CMAKE_THREAD_LIBS_INIT})

# Set rpath for the library
set_target_properties(RestApp PROPERTIES INSTALL_RPATH  "$ORIGIN/../:$ORIGIN")

# install library and header
install(TARGETS RestApp DESTINATION ${APP_INSTALL_PATH}/)
        
#install library directory
#install(DIRECTORY ${LibraryDirs}/* DESTINATION ${LIB_INSTALL_PATH}/)
