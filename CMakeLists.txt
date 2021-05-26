cmake_minimum_required(VERSION 3.12)
project(mpscplusplus
        DESCRIPTION "A KISS implementation of a queue to facilitate Multi Producer Single Consumer communication."
        HOMEPAGE_URL "https://github.com/JTriantafylos/mpscplusplus"
        LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic -Werror)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_library(mpscplusplus INTERFACE)
target_include_directories(mpscplusplus INTERFACE include)

option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

option(BUILD_DOCS "Build documentation" ON)
find_package(Doxygen)
if(BUILD_DOCS)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/doc")
        doxygen_add_docs(
                doc_mpscplusplus
                include
                ALL
                COMMENT "Generate API docs for mpscplusplus.")
    else()
        message("Doxygen need to be installed to generate documentation!")
    endif()
endif()