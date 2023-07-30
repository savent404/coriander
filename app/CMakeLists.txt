#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2023 Savent Gate
# SPDX-License-Identifier: All rights reserved

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C CXX)

include(cmake/CPM.cmake)
CPMAddPackage("gh:boost-ext/di@1.3.0")

###############
# coriander lib
###############
aux_source_directory(src/coriander sc_src)
set(coriander_src ${sc_src})
set(coriander_inc inc ${di_SOURCE_DIR}/include)
set(coriander_lib )

# build this library to check no specific backend is selected
add_library(coriander STATIC ${coriander_src})
target_include_directories(coriander PUBLIC ${coriander_inc})
target_link_libraries(coriander PUBLIC ${coriander_lib})

# coriander backend
file(GLOB_RECURSE backend_src src/coriander/backends/zephyr/*.cc)

target_sources(app PRIVATE src/main.cc ${coriander_src} ${backend_src})
target_include_directories(app PRIVATE
    inc
    ${coriander_inc})
target_link_libraries(app PRIVATE ${coriander_lib})
