################################################################################
# Copyright 1998-2018 by authors (see AUTHORS.txt)
#
#   This file is part of LuxCoreRender.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

include(FindPkgMacros)
getenv_path(LuxRays_DEPENDENCIES_DIR)

################################################################################
#
# Core dependencies
#
################################################################################

# Find threading library
find_package(Threads REQUIRED)

find_package(OpenImageIO REQUIRED)
include_directories(BEFORE SYSTEM ${OPENIMAGEIO_INCLUDE_DIR})
find_package(OpenEXR REQUIRED)

if(NOT APPLE)
    # Apple has these available hardcoded and matched in macos repo, see Config_OSX.cmake

    include_directories(BEFORE SYSTEM ${OPENEXR_INCLUDE_DIRS})
    find_package(TIFF REQUIRED)
    include_directories(BEFORE SYSTEM ${TIFF_INCLUDE_DIR})
    find_package(JPEG REQUIRED)
    include_directories(BEFORE SYSTEM ${JPEG_INCLUDE_DIR})
    find_package(PNG REQUIRED)
    include_directories(BEFORE SYSTEM ${PNG_PNG_INCLUDE_DIR})
	# Find Python Libraries
	find_package(PythonLibs 3.5)
endif()

find_program(PYSIDE_UIC NAME pyside-uic
		HINTS "${PYTHON_INCLUDE_DIRS}/../Scripts"
		PATHS "c:/Program Files/Python35/Scripts")


include_directories(${PYTHON_INCLUDE_DIRS})

# Find Boost
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT                  "${BOOST_SEARCH_PATH}")
#set(Boost_DEBUG                 ON)
set(Boost_MINIMUM_VERSION       "1.44.0")

set(Boost_ADDITIONAL_VERSIONS "1.47.0" "1.46.1" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0")

set(LUXRAYS_BOOST_COMPONENTS thread program_options filesystem serialization iostreams regex system python chrono serialization)
find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${LUXRAYS_BOOST_COMPONENTS})
if (NOT Boost_FOUND)
        # Try again with the other type of libs
        if(Boost_USE_STATIC_LIBS)
                set(Boost_USE_STATIC_LIBS OFF)
        else()
                set(Boost_USE_STATIC_LIBS ON)
        endif()
		message(STATUS "Re-trying with link static = ${Boost_USE_STATIC_LIBS}")
        find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${LUXRAYS_BOOST_COMPONENTS})
endif()

if (Boost_FOUND)
	include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	# Don't use old boost versions interfaces
	ADD_DEFINITIONS(-DBOOST_FILESYSTEM_NO_DEPRECATED)
	if (Boost_USE_STATIC_LIBS)
		ADD_DEFINITIONS(-DBOOST_STATIC_LIB)
		ADD_DEFINITIONS(-DBOOST_PYTHON_STATIC_LIB)
	endif()
endif ()


# OpenGL
find_package(OpenGL)

if (OPENGL_FOUND)
	include_directories(BEFORE SYSTEM ${OPENGL_INCLUDE_PATH})
endif()

# OpenCL
set(OPENCL_ROOT                "${OPENCL_SEARCH_PATH}")
find_package(OpenCL)

if (OPENCL_FOUND)
	include_directories(BEFORE SYSTEM ${OPENCL_INCLUDE_DIR} ${OPENCL_C_INCLUDE_DIR})
endif ()

# Intel Embree
set(EMBREE_ROOT                "${EMBREE_SEARCH_PATH}")
find_package(Embree REQUIRED)

if (EMBREE_FOUND)
	include_directories(BEFORE SYSTEM ${EMBREE_INCLUDE_PATH})
endif ()

# Intel TBB
set(TBB_ROOT                   "${TBB_SEARCH_PATH}")
find_package(TBB REQUIRED)

if (TBB_FOUND)
	include_directories(BEFORE SYSTEM ${TBB_INCLUDE_DIR})
endif ()

# Blosc
set(BLOSC_ROOT                   "${BLOSC_SEARCH_PATH}")
find_package(Blosc REQUIRED)

if (BLOSC_FOUND)
	include_directories(BEFORE SYSTEM ${TBB_INCLUDE_DIR})
endif ()

# OpenVDB
set(OPENVDB_ROOT               "${OPENVDB_SEARCH_PATH}")
find_package(OpenVDB REQUIRED)

if (OPENVDB_FOUND)
	include_directories(BEFORE SYSTEM ${OpenVDB_INCLUDE_DIR})
endif ()

# OpenMP
if(NOT APPLE)
	find_package(OpenMP)
	if (OPENMP_FOUND)
		MESSAGE(STATUS "OpenMP found - compiling with")
   		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
   		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	else()
		MESSAGE(WARNING "OpenMP not found - compiling without")
	endif()
endif()

# Find GTK 3.0 for Linux only (required by luxcoreui NFD)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	include_directories(${GTK3_INCLUDE_DIRS})
endif()

# Find BISON
IF (NOT BISON_NOT_AVAILABLE)
	find_package(BISON)
ENDIF (NOT BISON_NOT_AVAILABLE)

# Find FLEX
IF (NOT FLEX_NOT_AVAILABLE)
	find_package(FLEX)
ENDIF (NOT FLEX_NOT_AVAILABLE)
