cmake_minimum_required(VERSION 2.8)

# project name
project(offsite_yasksite_run CXX C)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_PREFIX_PATH ${yaskSite_DIR})
# find yaskSite
find_package(yaskSite REQUIRED CONFIG)
message(STATUS "Found yaskSite")

include_directories(${yaskSite_INCLUDE_DIR})
link_libraries(${yaskSite_LIBRARIES})
set(CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED On)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${yaskSite_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${yaskSite_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${yaskSite_CXX_FLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -xHOST -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -xHOST -g")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-write-strings")
endif()

find_package(OpenMP)

if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SOURCE offsite_run_parse.cpp offsite_helper.cpp)
add_executable(ys_A_il impl_variants/ys_A_il.cpp ${SOURCE})
add_executable(ys_A_li impl_variants/ys_A_li.cpp ${SOURCE})
add_executable(ys_F_il impl_variants/ys_F_il.cpp ${SOURCE})
