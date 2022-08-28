cmake_minimum_required(VERSION 3.24)

project(usdmar VERSION 0.1.0)

# Configurations
enable_testing()
include(FetchContent)

# Options
option(USDMAR_REST "Enables support for the rest subsolver (requires CPR package)" ON)
option(FETCH_CPR "Automatically fetch CPR dependent package" OFF)

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
	if (FETCH_CPR)
		FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
								 GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400) # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases
		FetchContent_MakeAvailable(cpr)
	else()
		find_package(cpr CONFIG REQUIRED)
	endif()
endif()

add_subdirectory(src)
