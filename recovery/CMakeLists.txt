# All rights reversed.

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(recovery)

aux_source_directory(src/ SRC_LIST)
target_sources(app PRIVATE ${SRC_LIST})
