cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(SpotiDash VERSION 1.0.0)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.hpp
    src/lib/*.hpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage(
    NAME curl
    GIT_REPOSITORY "https://github.com/curl/curl.git"
    GIT_TAG "curl-8_8_0"
    DOWNLOAD_ONLY YES
)

add_library(curl STATIC IMPORTED)

if (ANDROID)
    if (ANDROID_ABI STREQUAL "arm64-v8a")
        set(LIB_PLATFORM "android64")
    else()
        set(LIB_PLATFORM "android32")
    endif()
elseif (WIN32)
    set(LIB_PLATFORM "win64")
else()
    set(LIB_PLATFORM "macos")
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE CURL_STATICLIB=1)

# we are kinda leeching off of geode but we can always snatch those libraries in the future
# ^^^ this is from globed im just a leech of a leech
if (WIN32)
    set(CURL_LIBRARY "$ENV{GEODE_SDK}/loader/include/link/${LIB_PLATFORM}/libcurl.lib")
    set_target_properties(curl PROPERTIES IMPORTED_LOCATION "$ENV{GEODE_SDK}/loader/include/link/${LIB_PLATFORM}/libcurl.lib")
else()
    set(CURL_LIBRARY "$ENV{GEODE_SDK}/loader/include/link/${LIB_PLATFORM}/libcurl.a")
    set_target_properties(curl PROPERTIES IMPORTED_LOCATION "$ENV{GEODE_SDK}/loader/include/link/${LIB_PLATFORM}/libcurl.a")
endif()
set(CURL_INCLUDE_DIR "${curl_SOURCE_DIR}/include")

target_include_directories(${PROJECT_NAME} PRIVATE "${curl_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME} curl)

find_package(CURL)
if(CURL_FOUND)
    message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
    message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
    message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
else()
    message(STATUS "didnt find curl :(((((")
endif()

CPMAddPackage(
    NAME Boost
    VERSION 1.84.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
    URL_HASH SHA256=2e64e5d79a738d0fa6fb546c6e5c2bd28f88d268a2a080546f74e5ff98f29d0e
    OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_INCLUDE_LIBRARIES asio" # escape with \\\;
)
set(ASIO_INCLUDE_DIR "${boost_SOURCE_DIR}/libs/asio/include/boost")
include_directories("${boost_SOURCE_DIR}/libs/asio/include")
include_directories("${boost_SOURCE_DIR}/libs/asio/include/boost")

target_link_libraries(${PROJECT_NAME} Boost::asio)

CPMAddPackage("gh:CrowCpp/Crow#v1.2.0")

target_link_libraries(${PROJECT_NAME} Crow)

setup_geode_mod(${PROJECT_NAME})
