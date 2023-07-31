#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2023 Savent Gate
# SPDX-License-Identifier: All rights reserved
#-------------------------------------------------------------------------------

option(UNIT_TEST "Build tests" OFF)

cmake_minimum_required(VERSION 3.13.1)
if (UNIT_TEST)
    set(CMAKE_CXX_STANDARD 20)
else ()
    find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
endif (UNIT_TEST)

project(app LANGUAGES C CXX)

include(cmake/git_version.cmake)


###############
# CPM dependencies
###############
include(cmake/CPM.cmake)
CPMAddPackage("gh:boost-ext/di@1.3.0")
if (UNIT_TEST)
    CPMAddPackage("gh:google/googletest@1.12.0")
endif (UNIT_TEST)

###############
# coriander backend
###############
if (UNIT_TEST)
    set(backend_inc inc/coriander/backends/posix)
    file(GLOB_RECURSE backend_src src/coriander/backends/posix/*.cc)
else (UNIT_TEST)
    set(backend_inc inc/coriander/backends/zephyr)
    file(GLOB_RECURSE backend_src src/coriander/backends/zephyr/*.cc)
endif (UNIT_TEST)

###############
# coriander core
###############
aux_source_directory(src/coriander sc_src)
aux_source_directory(src/coriander/application sca_src)
set(coriander_src ${sc_src} ${sca_src})
set(coriander_inc inc ${di_SOURCE_DIR}/include ${backend_inc})
set(coriander_lib )
add_library(coriander STATIC ${coriander_src})
target_include_directories(coriander PUBLIC ${coriander_inc})
target_link_libraries(coriander PUBLIC ${coriander_lib})

###############
# app
###############
if (UNIT_TEST)
    aux_source_directory(tests ut_src ${backend_src})
    add_executable(ut ${ut_src})
    target_link_libraries(ut PRIVATE gtest_main coriander)
else ()
    target_sources(app PRIVATE src/main.cc ${coriander_src} ${backend_src})
    target_include_directories(app PRIVATE
        inc
        ${coriander_inc})
    target_link_libraries(app PRIVATE ${coriander_lib})
endif (UNIT_TEST)
