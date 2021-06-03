cmake_minimum_required(VERSION 3.12)
project(mpmcplusplus
        DESCRIPTION "A dead-simple implementation of a queue to facilitate Multi Producer Multi Consumer communication."
        HOMEPAGE_URL "https://github.com/JTriantafylos/mpmcplusplus"
        LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic -Werror)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_library(mpmcplusplus INTERFACE)
target_include_directories(mpmcplusplus INTERFACE include)

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
                doc_mpmcplusplus
                include
                ALL
                COMMENT "Generate API docs for mpmcplusplus.")
    else()
        message("Doxygen need to be installed to generate documentation!")
    endif()
endif()