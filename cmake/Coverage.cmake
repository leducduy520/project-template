# =============================================================================
# Code Coverage Configuration
# =============================================================================
# This module provides code coverage support using gcov/lcov for GCC/Clang
# or similar tools for other compilers.

function(add_coverage_flags)
    if(NOT ENABLE_COVERAGE)
        return()
    endif()

    message(STATUS "Enabling code coverage")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC coverage flags
        add_compile_options(--coverage)
        add_link_options(--coverage)
        message(STATUS "Using GCC coverage flags (--coverage)")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang coverage flags (compatible with gcov)
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage -fprofile-arcs -ftest-coverage)
        message(STATUS "Using Clang coverage flags (--coverage)")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # MSVC coverage flags
        add_compile_options(/Zi)
        add_link_options(/PROFILE)
        message(STATUS "Using MSVC coverage flags (/Zi /PROFILE)")
        message(WARNING "MSVC coverage support is limited. Consider using OpenCppCoverage.")
    else()
        message(WARNING "Code coverage not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
        return()
    endif()

    # Coverage requires debug symbols
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Coverage enabled: switching to Debug build type for coverage")
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    endif()
endfunction()

# =============================================================================
# Coverage Report Generation
# =============================================================================
function(setup_coverage_target)
    if(NOT ENABLE_COVERAGE)
        return()
    endif()

    # Check if lcov is available (for GCC/Clang)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)

        if(LCOV_PATH AND GENHTML_PATH)
            message(STATUS "Found lcov: ${LCOV_PATH}")
            message(STATUS "Found genhtml: ${GENHTML_PATH}")

            # Coverage directory
            set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage" CACHE PATH "Coverage output directory")
            set(COVERAGE_HTML_DIR "${COVERAGE_DIR}/html" CACHE PATH "Coverage HTML output directory")

            # Create coverage directory
            file(MAKE_DIRECTORY ${COVERAGE_DIR})
            file(MAKE_DIRECTORY ${COVERAGE_HTML_DIR})

            # Coverage data file
            set(COVERAGE_INFO_FILE "${COVERAGE_DIR}/coverage.info")

            # Add custom target to capture baseline (zero coverage)
            add_custom_target(coverage-baseline
                COMMAND ${CMAKE_COMMAND} -E echo "Resetting coverage counters..."
                COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR} --zerocounters || true
                COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage
                COMMENT "Resetting coverage counters to zero"
            )

            # Add custom target to check for coverage files (for debugging)
            add_custom_target(coverage-check
                COMMAND ${CMAKE_COMMAND} -E echo "Checking for coverage files in ${CMAKE_BINARY_DIR}..."
                COMMAND ${CMAKE_COMMAND} -E echo "Searching for .gcda files..."
                COMMAND find ${CMAKE_BINARY_DIR} -name "*.gcda" -type f 2>/dev/null | head -10 || ${CMAKE_COMMAND} -E echo "No .gcda files found. Make sure:"
                COMMAND ${CMAKE_COMMAND} -E echo "  1. Project was configured with -DENABLE_COVERAGE=ON"
                COMMAND ${CMAKE_COMMAND} -E echo "  2. Project was rebuilt after enabling coverage"
                COMMAND ${CMAKE_COMMAND} -E echo "  3. Tests were run after building"
                COMMENT "Checking coverage file generation"
            )

            # Add custom target to capture coverage data
            add_custom_target(coverage-capture
                COMMAND ${CMAKE_COMMAND} -E echo "Capturing coverage data from ${CMAKE_BINARY_DIR}..."
                COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR}
                COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR} --capture --output-file ${COVERAGE_INFO_FILE} --ignore-errors empty,mismatch,gcov --rc geninfo_unexecuted_blocks=1 || true
                COMMAND ${LCOV_PATH} --remove ${COVERAGE_INFO_FILE} '/usr/*' '*/vcpkg/*' '*/test/*' '*/external/*' '*/cmake/*' '*/out/*' --output-file ${COVERAGE_INFO_FILE} || true
                COMMENT "Capturing coverage data"
                DEPENDS coverage-test
            )

            # Add custom target to generate HTML report
            add_custom_target(coverage
                COMMAND ${GENHTML_PATH} -o ${COVERAGE_HTML_DIR} ${COVERAGE_INFO_FILE}
                COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated in: ${COVERAGE_HTML_DIR}/index.html"
                COMMENT "Generating HTML coverage report"
                DEPENDS coverage-capture
            )

            # Add custom target to run tests with coverage
            # Ensure test executable is built first, then run tests
            add_custom_target(coverage-test
                COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target PingPong-test
                COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
                COMMENT "Building test executable and running tests with coverage"
                DEPENDS coverage-baseline
            )

            # Combined target: run tests and generate report
            add_custom_target(coverage-all
                COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage-test
                COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage-capture
                COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage
                COMMENT "Running tests and generating coverage report"
                DEPENDS coverage-baseline
            )

            message(STATUS "Coverage targets available:")
            message(STATUS "  coverage-baseline  - Reset coverage counters")
            message(STATUS "  coverage-check     - Check for coverage files (debugging)")
            message(STATUS "  coverage-test      - Run tests with coverage")
            message(STATUS "  coverage-capture   - Capture coverage data")
            message(STATUS "  coverage           - Generate HTML report")
            message(STATUS "  coverage-all       - Run tests and generate report")
            message(STATUS "")
            message(STATUS "IMPORTANT: After enabling coverage, you must:")
            message(STATUS "  1. Reconfigure: cmake --preset <preset> -DENABLE_COVERAGE=ON")
            message(STATUS "  2. Rebuild: cmake --build --preset <preset>")
            message(STATUS "  3. Then run: cmake --build --preset <preset> --target coverage-all")
        else()
            message(WARNING "lcov or genhtml not found. Install lcov package for coverage reports.")
            message(WARNING "  On Ubuntu/Debian: sudo apt-get install lcov")
            message(WARNING "  On Fedora: sudo dnf install lcov")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS "For MSVC, consider using OpenCppCoverage for coverage reports")
        message(STATUS "  Download from: https://github.com/OpenCppCoverage/OpenCppCoverage")
    endif()
endfunction()

