# Copyright (C) 2025-present, Guanyou.Chen. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.21.1)
project(OpenMonitorKit)
set(TARGET_MONITOR open-monitor)
add_definitions(-D__MONITOR__="${TARGET_MONITOR}")

file(READ ${CMAKE_SOURCE_DIR}/VERSION version_contents)
string(REGEX MATCH "MAJOR=([0-9]+)" _ ${version_contents})
set(MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "MINOR=([0-9]+)" _ ${version_contents})
set(MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "PATCH=([0-9]+)" _ ${version_contents})
set(PATCH ${CMAKE_MATCH_1})
add_definitions(-D__MONITOR_VERSION__="${MAJOR}.${MINOR}.${PATCH}")

function(get_git_revision_variable VAR)
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_VERSION
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(${VAR} "${GIT_VERSION}" PARENT_SCOPE)
    else()
        set(${VAR} "unknown" PARENT_SCOPE)
    endif()
endfunction()
get_git_revision_variable(GIT_VERSION)
add_definitions(-D__GIT_VERSION__="${GIT_VERSION}")

string(TOUPPER "${CMAKE_BUILD_TARGET}" TARGET_BUILD_PLATFORM)
add_definitions(-D__${TARGET_BUILD_PLATFORM}__)

if (ANDROID_ABI STREQUAL "arm64-v8a")
add_definitions(-D__ARM64__)
add_definitions(-D__LP64__)
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
add_definitions(-D__ARM__)
add_definitions(-D__LP32__)
elseif(ANDROID_ABI STREQUAL "armeabi")
add_definitions(-D__ARM__)
add_definitions(-D__LP32__)
elseif(ANDROID_ABI STREQUAL "x86_64")
add_definitions(-D__X86_64__)
add_definitions(-D__LP64__)
elseif(ANDROID_ABI STREQUAL "x86")
add_definitions(-D__X86__)
add_definitions(-D__LP32__)
elseif(ANDROID_ABI STREQUAL "riscv64")
add_definitions(-D__RISCV64__)
add_definitions(-D__LP64__)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
add_definitions(-D__LP64__)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
add_definitions(-D__LP32__)
else()
add_definitions(-D__LP64__)
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" TARGET_BUILD_TYPE)
add_definitions(-D__PARSER_${TARGET_BUILD_TYPE}__)

string(TOUPPER "${CMAKE_BUILD_TARGET_PAGESIZE}" TARGET_BUILD_PAGESIZE)

if (TARGET_BUILD_TYPE STREQUAL "RELEASE")
set(OPTIMIZED_LEVEL "-O3")
else()
set(OPTIMIZED_LEVEL "-O2")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 ${OPTIMIZED_LEVEL} -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic -ldl")

include_directories(monitor)
add_library(${TARGET_MONITOR} SHARED
            monitor/monitor_main.cpp)

find_library(log-lib log)
target_link_libraries(${TARGET_MONITOR} ${log-lib})

if (TARGET_BUILD_PAGESIZE STREQUAL "16K")
target_link_options(${TARGET_MONITOR} PRIVATE "-Wl,-z,max-page-size=16384")
endif()
