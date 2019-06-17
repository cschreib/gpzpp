cmake_minimum_required(VERSION 2.6)
project(GPz++-superbuild C CXX)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected (-DCMAKE_BUILD_TYPE=...)")
    message(STATUS "Default to Release.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

include(ExternalProject)

# Set default install dir to "here"
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "No install directory selected (-DCMAKE_INSTALL_PREFIX=...)")
    message(STATUS "Default to ${PROJECT_SOURCE_DIR}/bin")
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "default install path" FORCE)
endif()

# Configure and build Eigen3
ExternalProject_Add(Eigen3
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/Eigen3
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/external/Eigen3
    GIT_REPOSITORY https://github.com/eigenteam/eigen-git-mirror
    GIT_TAG 3.3.7
    CMAKE_ARGS
        -DBYPASS_ELEMENTS=ON
        -DGPZ_NO_GSL=ON
        -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/external
        -DCMAKE_BUILD_TYPE=Release)

# Configure and build PHZ_GPz
ExternalProject_Add(PHZ_GPz
    DEPENDS Eigen3
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/PHZ_GPz
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/external/PHZ_GPz
    GIT_REPOSITORY https://github.com/cschreib/PHZ_GPz.git
    GIT_TAG develop
    CMAKE_ARGS
        -DBYPASS_ELEMENTS=ON
        -DGPZ_NO_GSL=ON
        -DGPZ_STATIC=ON
        -DEIGEN3_INCLUDE_DIR=${PROJECT_SOURCE_DIR}/external/include/eigen3
        -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/external
        -DCMAKE_BUILD_TYPE=Release)

# Configure and build vif
ExternalProject_Add(vif
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/vif
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/external/vif
    GIT_REPOSITORY https://github.com/cschreib/vif.git
    GIT_TAG 0297ae9dde2dd13d983e1a86da189a95066fe7c7
    CMAKE_ARGS
        -DNO_REFLECTION=ON
        -DNO_FFTW=ON
        -DNO_LAPACK=ON
        -DNO_GSL=ON
        -DNO_WCSLIB=ON
        -DNO_CFITSIO=ON
        -DVIF_INPLACE_BUILD=ON
        -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/external
        -DCMAKE_BUILD_TYPE=Release)

# Configure and build GPz++
ExternalProject_Add(gpz++
    DEPENDS vif PHZ_GPz
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/src
    LIST_SEPARATOR "|"
    CMAKE_ARGS
        -DEIGEN3_INCLUDE_DIR=${PROJECT_SOURCE_DIR}/external/include/eigen3
        -DGPZ_ROOT_DIR=${PROJECT_SOURCE_DIR}/external
        -DVIF_ROOT_DIR=${PROJECT_SOURCE_DIR}/external
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
        -DCMAKE_MODULE_PATH_VIF=${PROJECT_SOURCE_DIR}/external/vif/cmake
        -DCMAKE_MODULE_PATH_GPZ=${PROJECT_SOURCE_DIR}/external/PHZ_GPz/cmake
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

install(PROGRAMS
    ${CMAKE_BINARY_DIR}/bin/gpz++
    DESTINATION bin COMPONENT runtime)
