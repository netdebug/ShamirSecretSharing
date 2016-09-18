cmake_minimum_required(VERSION 2.8)

project(ShamirSecretSharing)

include(CTest)

# Find GMP on the system
find_path(GMP_INCLUDE_DIR NAMES gmp.h)
if(NOT GMP_INCLUDE_DIR)
    message(FATAL_ERROR "libgmp header files could not be found!")
endif()
find_library(GMP_LIBRARIES gmp)
if(NOT EXISTS ${GMP_LIBRARIES})
    message(FATAL_ERROR "libgmp library could not be found!")
endif()

include_directories(${GMP_INCLUDES})
include_directories(src)

# Base common flags between GCC and Clang
if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR
   ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_C_FLAGS "-std=c89 -Wall -Wextra -Werror")
endif()
if(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything")
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
