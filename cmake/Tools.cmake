if(${BUILD_CMAKE_FORMAT})
    set(ROOT_CMAKE_FILES "${CMAKE_SOURCE_DIR}/CMakeLists.txt")
    file(GLOB_RECURSE CMAKE_FILES_TXT "**/CMakeLists.txt")
    file(GLOB_RECURSE CMAKE_FILES_C "cmake/*.cmake")
    list(FILTER CMAKE_FILES_TXT EXCLUDE REGEX
        "${CMAKE_SOURCE_DIR}/(out/|external/|tools/|.github/)+"
    )
    set(CMAKE_FILES ${ROOT_CMAKE_FILES} ${CMAKE_FILES_TXT} ${CMAKE_FILES_C})
    find_program(CMAKE_FORMAT cmake-format)

    if(CMAKE_FORMAT)
        message(STATUS "Added Cmake Format")
        set(FORMATTING_COMMANDS)

        foreach(cmake_file ${CMAKE_FILES})
            list(
                APPEND
                FORMATTING_COMMANDS
                COMMAND
                cmake-format
                -c
                ${CMAKE_SOURCE_DIR}/.cmake-format.yaml
                -i
                ${cmake_file}
            )
        endforeach()

        add_custom_target(
            run_cmake_format COMMAND ${FORMATTING_COMMANDS}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
        set_target_properties(run_cmake_format PROPERTIES FOLDER "Custom target")
    else(CMAKE_FORMAT)
        message(WARNING "CMAKE_FORMAT NOT FOUND")
    endif(CMAKE_FORMAT)
endif(${BUILD_CMAKE_FORMAT})

if(${BUILD_CLANG_FORMAT})
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    if(NOT ${Python_FOUND})
        return()
    endif()

    file(GLOB_RECURSE CMAKE_FILES_CC "*/*.cc")
    file(GLOB_RECURSE CMAKE_FILES_CPP "*/*.cpp")
    file(GLOB_RECURSE CMAKE_FILES_H "*/*.h")
    file(GLOB_RECURSE CMAKE_FILES_HPP "*/*.hpp")
    set(CPP_FILES ${CMAKE_FILES_CC} ${CMAKE_FILES_CPP} ${CMAKE_FILES_H}
        ${CMAKE_FILES_HPP}
    )
    list(FILTER CPP_FILES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/(out|external)/.*")
    find_program(CLANGFORMAT clang-format REQUIRED)

    if(CLANGFORMAT)
        message(STATUS "Added Clang Format")
        add_custom_target(
            run_clang_format
            COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/tools/run-clang-format.py ${CPP_FILES}
            --in-place
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            USES_TERMINAL
        )
        set_target_properties(run_clang_format PROPERTIES FOLDER "Custom target")
    else()
        message(WARNING "CLANGFORMAT NOT FOUND")
    endif()
endif()
