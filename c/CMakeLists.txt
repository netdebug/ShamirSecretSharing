cmake_minimum_required(VERSION 2.8.4)

project(ShamirSecretSharing)

set(CMAKE_BUILD_TYPE "Release")

include(CTest)

# Find GMP on the system. Use GMP_PATH as a hint for this CMake script
find_path(GMP_INCLUDE_DIR NAMES gmp.h PATHS ENV GMP_PATH)
if(NOT GMP_INCLUDE_DIR)
	message(FATAL_ERROR "GNU Multiprecision (GMP) library header files could not be found!")
endif()
find_library(GMP_LIBRARY gmp PATHS ENV GMP_PATH)
if(NOT EXISTS ${GMP_LIBRARY})
	message(FATAL_ERROR "GNU Multiprecision (GMP) library could not be found!")
endif()
get_filename_component(GMP_LIBRARY_DIR ${GMP_LIBRARY} PATH)

include_directories(${GMP_INCLUDE_DIR})
link_directories(${GMP_LIBRARY_DIR})
include_directories(src)

# Base common flags between GCC and Clang
if(UNIX)
	if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR
		${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
		set(CMAKE_C_FLAGS "-std=c89 -Wall -Wextra -Werror")
	endif()
	if(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything")
	endif()
elseif(WIN32)
	if (MSVC)
		set(CMAKE_C_FLAGS "/Za /W4")
	endif()
endif()

# Build the library
add_library(shamir STATIC src/shamir.c)

# Build the tests
add_executable(test1 tests/test1.c)
target_link_libraries(test1 shamir gmp)
add_executable(test2 tests/test2.c)
target_link_libraries(test2 shamir gmp)

add_test(NAME test_test1 COMMAND test1)
set_tests_properties(test_test1 PROPERTIES DEPENDS test1)
add_test(NAME test_test2 COMMAND test2)
set_tests_properties(test_test2 PROPERTIES DEPENDS test2)

# Install the stuff
install(FILES src/shamir.h DESTINATION include)
install(TARGETS shamir DESTINATION lib)
