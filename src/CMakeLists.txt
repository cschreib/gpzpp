cmake_minimum_required(VERSION 2.6)
project(gpz++ C CXX)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release (-DCMAKE_BUILD_TYPE=...)")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(NO_REFLECTION ON)
set(NO_FFTW ON)
set(NO_LAPACK ON)
set(NO_GSL ON)
set(NO_WCSLIB ON)
set(NO_CFITSIO ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_VIF} ${CMAKE_MODULE_PATH_GPZ})

find_package(GPz REQUIRED)
find_package(vif REQUIRED)

# Figure out git hash, if any
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  RESULT_VARIABLE GIT_COMMAND_SUCCESS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (GIT_COMMAND_SUCCESS EQUAL 0)
    add_definitions(-DGPZPP_GIT_HASH="${GIT_COMMIT_HASH}")
else()
    add_definitions(-DGPZPP_GIT_HASH="")
endif()

# Include dependencies
include_directories(${VIF_INCLUDE_DIRS})
message(STATUS ${GPZ_INCLUDE_DIRS})
include_directories(${GPZ_INCLUDE_DIRS})

# Build gpz++
add_executable(gpz++
  gpz++.cpp
  gpz++-read_input.cpp
  gpz++-write_output.cpp)

target_link_libraries(gpz++ ${VIF_LIBRARIES})
target_link_libraries(gpz++ ${GPZ_LIBRARIES})
install(TARGETS gpz++ DESTINATION bin)
