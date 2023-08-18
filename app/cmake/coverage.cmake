cmake_minimum_required(VERSION 3.18)

if(CMAKE_BUILD_TYPE STREQUAL "coverage" OR CODE_COVERAGE)
    if("${CMAKE_C_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
        message("Building with llvm Code Coverage Tools")
        find_file(LLVM_COV_PATH NAME llvm-cov
            PATSH "~/.local/bin" "/usr/local/bin" "/usr/bin"
            REQUIRED)
        message("Using llvm-code: ${LLVM_COV_PATH}")
        # Warning/Error messages
        if(NOT LLVM_COV_PATH)
            message(FATAL_ERROR "llvm-cov not found! Aborting.")
        endif()

        # set Flags
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")

    elseif(CMAKE_COMPILER_IS_GNUCXX)
        message("Building with lcov Code Coverage Tools")

        # Warning/Error messages
        if(NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
            message(WARNING "Code coverage results with an optimized (non-Debug) build may be misleading")
        endif()
        if(NOT LCOV_PATH)
            message(FATAL_ERROR "lcov not found! Aborting...")
        endif()
        if(NOT GENHTML_PATH)
            message(FATAL_ERROR "genhtml not found! Aborting...")
        endif()

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    else()
        message(FATAL_ERROR "Code coverage requires Clang or GCC. Aborting.")
    endif()
endif()

macro(add_llvm_cov  TARGET_NAME)
add_custom_target(${TARGET_NAME}-ccov-preprocessing
    COMMAND LLVM_PROFILE_FILE=${TARGET_NAME}.profraw $<TARGET_FILE:${TARGET_NAME}>
    COMMAND llvm-profdata merge -sparse ${TARGET_NAME}.profraw -o ${TARGET_NAME}.profdata
    DEPENDS ${TARGET_NAME})

add_custom_target(${TARGET_NAME}-ccov-show
    COMMAND llvm-cov show $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata -show-line-counts-or-regions
    DEPENDS ${TARGET_NAME}-ccov-preprocessing)

add_custom_target(${TARGET_NAME}-ccov-report
    COMMAND llvm-cov report $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata
    DEPENDS ${TARGET_NAME}-ccov-preprocessing)

add_custom_target(${TARGET_NAME}-ccov
    COMMAND llvm-cov show $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata -show-line-counts-or-regions -output-dir=${CMAKE_BINARY_DIR}/${TARGET_NAME}-llvm-cov -format="html"
    DEPENDS ${TARGET_NAME}-ccov-preprocessing)

add_custom_command(TARGET ${TARGET_NAME}-ccov POST_BUILD
    COMMAND ;
    COMMENT "Open ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}-llvm-cov/index.html in your browser to view the coverage report."
)	
endmacro(add_llvm_cov)
