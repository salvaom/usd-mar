cmake_minimum_required(VERSION 3.24)

project(usdmar VERSION 0.1.0)

# Configurations
enable_testing()
include(FetchContent)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)


# Options
option(USDMAR_REST "Enables support for the rest subsolver (requires CPR package)" ON)
option(USDMAR_SUBPROCESS "Enables support for the subprocess subsolver (requires subprocess.h)" ON)
option(DOWNLOAD_CPR "Automatically fetch CPR dependent package" OFF)
option(DOWNLOAD_SUBPROCESSH "Automatically fetch subprocess.h dependent package" ON)

# Resolver schemes
if(NOT DEFINED RESOLVER_SCHEMES)
	set(RESOLVER_SCHEMES "mr")
endif()
separate_arguments(RESOLVER_SCHEMES)

# Dependencies
find_package(pxr REQUIRED arch as)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# CPR
if (USDMAR_REST)
	add_compile_definitions(USDMAR_REST)
	if (DOWNLOAD_CPR)
		FetchContent_Declare(cpr
							GIT_REPOSITORY https://github.com/libcpr/cpr.git
							GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400)
		FetchContent_MakeAvailable(cpr)
	else()
		find_package(cpr CONFIG REQUIRED)
	endif()
endif()

INCLUDE(FindPackageHandleStandardArgs)

# subprocess.h
if (USDMAR_SUBPROCESS)
	add_compile_definitions(USDMAR_SUBPROCESS)
	if (DOWNLOAD_SUBPROCESSH)
		set(SUBPROCESSH_HEADER_PATH "${CMAKE_BINARY_DIR}/_deps/subprocess_h/subprocess.h")
		get_filename_component(SUBPROCESSH_INCLUDE "${SUBPROCESSH_HEADER_PATH}" DIRECTORY )
		file(DOWNLOAD
			"https://raw.githubusercontent.com/sheredom/subprocess.h/master/subprocess.h"
			${SUBPROCESSH_HEADER_PATH}
		)
	endif()
	find_package(SUBPROCESSH REQUIRED)
endif()


add_subdirectory(src)
