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

aux_source_directory(src/coriander sc_src)
aux_source_directory(src/coriander/application sca_src)

target_sources(app PRIVATE src/main.cc ${sc_src} ${sca_src})
target_include_directories(app PRIVATE
    inc
    ${di_SOURCE_DIR}/include)
