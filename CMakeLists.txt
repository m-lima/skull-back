cmake_minimum_required(VERSION 3.10)
project(skull CXX)

##------------------------------------------------------------------------------
## Initialization
##

include("${CMAKE_SOURCE_DIR}/Init.cmake")

# Force the use of std::string_view (instead of experimental)
list(APPEND DEFINITIONS "-DASIO_HAS_STD_STRING_VIEW")

##------------------------------------------------------------------------------
## Dependencies
##

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
      "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_install(
    BUILD missing
    CONANFILE conanfile.txt
)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

list(APPEND LIBRARIES CONAN_PKG::mfl)
list(APPEND LIBRARIES CONAN_PKG::boost)
list(APPEND LIBRARIES CONAN_PKG::restinio)
list(APPEND LIBRARIES CONAN_PKG::spdlog)

##------------------------------------------------------------------------------
## Sources
##

list(APPEND SOURCES
  ${SRC_DIR}/context.cpp
  ${SRC_DIR}/file_handle.cpp
  ${SRC_DIR}/server.cpp
  ${SRC_DIR}/storage.cpp
)

list(APPEND HEADERS
  ${SRC_DIR}/constants.hpp
)

##------------------------------------------------------------------------------
## Targets
##

add_definitions(${DEFINITIONS})

add_library(skull-lib STATIC ${SOURCES})
target_link_libraries(skull-lib PUBLIC ${LIBRARIES})
target_include_directories(skull-lib PUBLIC ${INCLUDE_DIRS})

# Copy the resources
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/res)
file(GLOB RESOURCES ${SRC_DIR}/res/*)
file(COPY ${RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/bin/res)

##------------------------------------------------------------------------------
## Main
##

add_executable(skull ${SRC_DIR}/main.cpp)
target_link_libraries(skull PRIVATE skull-lib)

##------------------------------------------------------------------------------
## Tests
##

include("${CMAKE_SOURCE_DIR}/Tests.cmake")
