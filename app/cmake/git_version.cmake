
find_package(Git)

if (GIT_FOUND)
    set(USE_GIT 1)
else(GIT_FOUND)
    set(USE_GIT 0)
endif(GIT_FOUND)

if (USE_GIT)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Git version: ${GIT_VERSION}")
else(USE_GIT)
    set(GIT_VERSION "unknown")
endif(USE_GIT)

configure_file(inc/app_version.h.in ${CMAKE_BINARY_DIR}/app_version.h)

include_directories(${CMAKE_BINARY_DIR})