#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2023 Savent Gate
# SPDX-License-Identifier: All rights reserved
#-------------------------------------------------------------------------------

option(UNIT_TEST "Build tests" OFF)

cmake_minimum_required(VERSION 3.13.1)
if (UNIT_TEST)
    include(cmake/coverage.cmake)
else ()
    find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
endif (UNIT_TEST)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

project(app LANGUAGES C CXX)

include(cmake/git_version.cmake)


###############
# CPM dependencies
###############
include(cmake/CPM.cmake)
CPMAddPackage("gh:boost-ext/di@1.3.0")
CPMAddPackage("gh:aantron/better-enums#0.11.3")
CPMAddPackage("gh:boostorg/circular_buffer#boost-1.76.0")
CPMAddPackage("gh:boostorg/assert#boost-1.76.0")
CPMAddPackage("gh:boostorg/config#boost-1.76.0")
CPMAddPackage("gh:boostorg/concept_check#boost-1.76.0")
CPMAddPackage("gh:boostorg/core#boost-1.76.0")
CPMAddPackage("gh:boostorg/move#boost-1.76.0")
CPMAddPackage("gh:boostorg/preprocessor#boost-1.76.0")
CPMAddPackage("gh:boostorg/static_assert#boost-1.76.0")
CPMAddPackage("gh:boostorg/throw_exception#boost-1.76.0")
CPMAddPackage("gh:boostorg/type_traits#boost-1.76.0")
if (UNIT_TEST)
    CPMAddPackage("gh:google/googletest@1.12.0")
endif (UNIT_TEST)

###############
# coriander backend
###############
if (UNIT_TEST)
    set(backend_inc inc/coriander/backends)
    file(GLOB_RECURSE backend_src src/coriander/backends/posix/*.cc)
else (UNIT_TEST)
    set(backend_inc inc/coriander/backends)
    file(GLOB_RECURSE backend_src src/coriander/backends/zephyr/*.cc)
endif (UNIT_TEST)

###############
# coriander core
###############
aux_source_directory(src/coriander sc_src)
aux_source_directory(src/coriander/application sca_src)
aux_source_directory(src/coriander/motorctl scm_src)
aux_source_directory(src/coriander/protos scp_src)
set(coriander_src ${sc_src} ${sca_src} ${scm_src} ${scp_src})
set(coriander_inc inc ${di_SOURCE_DIR}/include ${backend_inc} ${better-enums_SOURCE_DIR})
set(coriander_lib boost_circular_buffer)
add_library(coriander STATIC ${coriander_src})
target_include_directories(coriander PUBLIC ${coriander_inc})
target_link_libraries(coriander PUBLIC ${coriander_lib})

###############
# app
###############
if (UNIT_TEST)
    aux_source_directory(tests ut_src ${backend_src})
    add_executable(ut ${ut_src})
    add_llvm_cov(ut)
    target_link_libraries(ut PRIVATE gtest_main gmock coriander ${coriander_lib})
    target_include_directories(ut PRIVATE . ${coriander_inc})
else ()
    aux_source_directory(boards/${CONFIG_BOARD} board_src)
    target_sources(app PRIVATE src/main_zephyr.cc ${coriander_src} ${backend_src} ${board_src})
    target_include_directories(app PRIVATE
        inc
        ${coriander_inc})
    target_link_libraries(app PRIVATE ${coriander_lib})
endif (UNIT_TEST)
