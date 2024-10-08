cmake_minimum_required(VERSION 3.26)
project(system)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE "Debug")

set(PROJECT_SOURCE_TARGET ${PROJECT_NAME}_source_target)

include_directories(../../include)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

set(THIRD_LIB_NAME signal_stack)
find_library(third_lib_${THIRD_LIB_NAME} ${THIRD_LIB_NAME} ../../lib)
if (third_lib_${THIRD_LIB_NAME})
    target_link_libraries(${PROJECT_NAME}
        PUBLIC ${third_lib_${THIRD_LIB_NAME}}
    )
endif()

add_subdirectory(src)
target_link_libraries(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_TARGET}
)