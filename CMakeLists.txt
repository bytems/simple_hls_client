cmake_minimum_required(VERSION 3.17)

message("-- CMake version is ${CMAKE_VERSION}")
message("-- CMake project is hls_fetch_and_sort")
project(hls_fetch_and_sort)

# Add path to cmake modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(setup_configurations)   # defines build configurations - needs to be first
include(set_build_flags)        # sets compiler and linker flags


add_executable(hls_fetch_and_sort)
target_sources(hls_fetch_and_sort
        PUBLIC
            main.cpp
            HLSFetcher.h
            HLSWriter.h
            HLSTagParser.h
            StreamInfParser.h
            MediaParser.h
            iFrameParser.h
            M3U8Parser.h
)


# Add support for CURL, OpenSSL
find_package(CURL REQUIRED)
include_directories(
        ${CURL_INCLUDE_DIR}
)

target_link_libraries(hls_fetch_and_sort
        PRIVATE
            ${CURL_LIBRARIES}
)

