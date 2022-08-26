cmake_minimum_required(VERSION 3.8)

project(usdmar VERSION 0.1.0)

if(NOT DEFINED RESOLVER_SCHEMAS)
	set(RESOLVER_SCHEMAS "mr")
endif()

separate_arguments(RESOLVER_SCHEMAS)

enable_testing()

find_package(pxr REQUIRED arch as)
find_package(cpr CONFIG REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

add_subdirectory(src)
