cmake_minimum_required(VERSION 3.14)
project(tool)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_subdirectory(src)

if(WIN32)
    find_path(NPCAP_INCLUDE_DIR pcap.h PATHS "C:/npcap-sdk/Include")
    find_library(NPCAP_LIB wpcap PATHS "C:/npcap-sdk/Lib/x64")
    target_link_libraries(toollib PUBLIC ${NPCAP_LIB} ws2_32)
    target_include_directories(toollib PUBLIC ${NPCAP_INCLUDE_DIR})
endif()

add_executable(testtool test/main.cpp)
target_link_libraries(testtool toollib Catch2::Catch2WithMain)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_FILE_PATTERNS *.h)
    set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generating documentation"
    )
endif()