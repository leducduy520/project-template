function(add_gen_doc)
    if(NOT ENABLE_DOCUMENT_TARGET)
        return()
    endif()

    find_package(Doxygen REQUIRED COMPONENTS dot doxygen)

    if(Doxygen_FOUND)
        set(DOCS_HTML_DIR
            ${DOCS_DIR}/html
            CACHE PATH "project html directory"
        )
        set(DOCS_GRAPH_DIR
            ${DOCS_DIR}/graph
            CACHE PATH "project graph directory"
        )

        add_custom_command(
            OUTPUT ${DOCS_HTML_DIR}/index.html
            COMMAND doxygen ./docCfg
            DEPENDS ${DOCS_DIR}/docCfg
            WORKING_DIRECTORY ${DOCS_DIR}
            VERBATIM USES_TERMINAL
        )

        add_custom_target(
            gen_doc
            COMMENT "Generate project document"
            DEPENDS ${DOCS_HTML_DIR}/index.html
            SOURCES ${DOCS_DIR}/docCfg
        )

        add_custom_command(
            OUTPUT ${DOCS_GRAPH_DIR}/${GRAPH_OUTPUT_NAME}.png
            COMMAND dot -v -Tpng graph.dot -o ${GRAPH_OUTPUT_NAME}.png
            DEPENDS ${DOCS_GRAPH_DIR}/graph.dot
            WORKING_DIRECTORY ${DOCS_GRAPH_DIR}
            VERBATIM USES_TERMINAL
        )

        add_custom_target(
            gen_graph
            COMMENT "Generate project graph dependencies"
            DEPENDS ${DOCS_GRAPH_DIR}/${GRAPH_OUTPUT_NAME}.png
        )

        if(ENABLE_MOVE_DOCS_TO_INSTALL)
            add_custom_command(
                TARGET gen_graph
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${DOCS_DIR}/html ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                VERBATIM USES_TERMINAL
            )
        endif()
    endif()
endfunction()

function(read_deps_json)
    file(READ ${CMAKE_SOURCE_DIR}/deps.json DEPS_JSON_STRING)

    string(JSON DEPS_LENGTH LENGTH ${DEPS_JSON_STRING})
    math(EXPR LAST_INDX "${DEPS_LENGTH} - 1" OUTPUT_FORMAT DECIMAL)
    set(PKG_LAST_INDEX
        ${LAST_INDX}
        PARENT_SCOPE
    )

    foreach(ITR RANGE ${LAST_INDX})
        string(JSON LABEL GET ${DEPS_JSON_STRING} ${ITR} label)
        list(APPEND LABELS ${LABEL})
        string(JSON REPO GET ${DEPS_JSON_STRING} ${ITR} repo)
        list(APPEND REPOS ${REPO})
        string(JSON BRANCH GET ${DEPS_JSON_STRING} ${ITR} branch)
        list(APPEND BRANCHES ${BRANCH})
    endforeach()

    set(PKG_BRANCHES
        ${BRANCHES}
        PARENT_SCOPE
    )
    set(PKG_LABELS
        ${LABELS}
        PARENT_SCOPE
    )
    set(PKG_REPOS
        ${REPOS}
        PARENT_SCOPE
    )
endfunction(read_deps_json)

function(build_external_project)
    set(multiValueArgs REPOS BRANCHES LABELS)
    set(oneValueArgs SIZE)

    cmake_parse_arguments(EX_PROJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(EX_PROJ_LIBDIR ${EXTERNAL_DIR}/${CMAKE_INSTALL_LIBDIR})

    file(WRITE ${EXTERNAL_DIR}/CMakeLists.txt
        "cmake_minimum_required(VERSION 3.27)\nproject(external)\ninclude(ExternalProject)\nadd_subdirectory(${CMAKE_INSTALL_LIBDIR})"
    )

    if(NOT EXISTS ${EX_PROJ_LIBDIR})
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${EX_PROJ_LIBDIR})
    endif()

    file(WRITE ${EX_PROJ_LIBDIR}/CMakeLists.txt "project(${CMAKE_INSTALL_LIBDIR})\n")

    foreach(ITR RANGE ${EX_PROJ_SIZE})
        list(GET EX_PROJ_REPOS ${ITR} EX_PROJ_REPO)
        list(GET EX_PROJ_BRANCHES ${ITR} EX_PROJ_BRANCH)
        list(GET EX_PROJ_LABELS ${ITR} EX_PROJ_LABEL)

        string(TOLOWER ${EX_PROJ_LABEL} EX_PROJ_LABEL_LOWER)
        set(EX_SUB_PROJ_DIR ${EX_PROJ_LIBDIR}/${EX_PROJ_LABEL_LOWER})

        if(NOT EXISTS ${EX_SUB_PROJ_DIR})
            execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${EX_SUB_PROJ_DIR})
        endif()

        configure_file(${CMAKE_SOURCE_DIR}/cmake/ExternalCMakeLists.txt.in ${EX_SUB_PROJ_DIR}/CMakeLists.txt @ONLY)

        file(APPEND ${EX_PROJ_LIBDIR}/CMakeLists.txt "add_subdirectory(${EX_PROJ_LABEL_LOWER})\n")
    endforeach()

    configure_file(${CMAKE_SOURCE_DIR}/cmake/ExternalCMakePresets.json.in ${EXTERNAL_DIR}/CMakePresets.json)

    execute_process(COMMAND ${CMAKE_COMMAND} --no-warn-unused-cli --preset=${PRESET} WORKING_DIRECTORY ${EXTERNAL_DIR})

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build --preset=${PRESET}
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE ERROR
    )

    if(NOT(${RESULT} EQUAL 0))
        message(WARNING "Output message: ${OUTPUT}")
        message(FATAL_ERROR "Error message: ${ERROR}")
    endif()
endfunction()
