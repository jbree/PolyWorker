cmake_minimum_required(VERSION 3.8)

# Project Setup ###############################################################

project(PolyWorker VERSION 1.0.0)

set(AUTHOR_EMAIL jbreeden@me.com)

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)


# Source Files ################################################################

configure_file(
    templates/include/GlobalConfiguration.hh.in
    ${GENERATED_DIR}/include/GlobalConfiguration.hh
    @ONLY
    )

set(SOURCES
#    src/main.cc
    src/Flag.cc
    src/Lock.cc
    )

set(HEADERS
    include/Flag.hh
    include/Lock.hh
    include/PolyWorker.hh
    include/WorkerThread.hh
    ${GENERATED_DIR}/include/GlobalConfiguration.hh
    )

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        include
    PRIVATE
        ${GENERATED_DIR}/include
    )

# Warnings & Compiler Features ################################################

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

target_compile_features(${PROJECT_NAME}
    PUBLIC
        cxx_std_11
    )

# Documentation ###############################################################

find_package(Doxygen)

if (Doxygen_FOUND)
  configure_file(
      templates/Doxyfile.in
      ${GENERATED_DIR}/Doxyfile
      @ONLY
  )

  add_custom_target(doc
      COMMAND ${DOXYGEN_EXECUTABLE} ${GENERATED_DIR}/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
endif()


# Google Test #################################################################

include(CTest)
enable_testing()

configure_file(templates/googletest/CMakeLists.txt.in external/googletest/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/googletest )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external/googletest )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

add_subdirectory(
    ${CMAKE_BINARY_DIR}/external/googletest-src
    ${CMAKE_BINARY_DIR}/external/googletest-build
    EXCLUDE_FROM_ALL)

add_subdirectory(test)
