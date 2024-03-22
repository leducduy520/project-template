function(add_gen_doc)
    if(NOT ENABLE_DOCUMENT_TARGET)
        return()
    endif()

    find_package(Doxygen REQUIRED)

    if(Doxygen_FOUND)
        set(DOCS_HTML_DIR ${DOCS_DIR}/html CACHE PATH "project html directory")
        set(DOCS_GRAPH_DIR ${DOCS_DIR}/graph CACHE PATH "project graph directory")

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
    endif()
endfunction()

function(build_external_project)

    set(oneValueArgs REPO BRANCH LABEL)
    cmake_parse_arguments(
        EX_PROJ
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

    message(STATUS "repo ${EX_PROJ_REPO}")
    message(STATUS "branch ${EX_PROJ_BRANCH}")
    message(STATUS "name ${EX_PROJ_LABEL}")

    set(RESULT)
    set(OUTPUT)
    set(ERROR)

    set(EX_PROJ_LABEL_LOWER)
    string(TOLOWER ${EX_PROJ_LABEL} EX_PROJ_LABEL_LOWER)

    if(NOT EXISTS ${EXTERNAL_DIR}/${EX_PROJ_LABEL_LOWER})
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${EXTERNAL_DIR}/${EX_PROJ_LABEL_LOWER})

        configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/ExternalCMakeLists.txt.in
        ${EXTERNAL_DIR}/${EX_PROJ_LABEL_LOWER}/CMakeLists.txt
        @ONLY
        )
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --fresh -S . -B ${EXTERNAL_BINARY_DIR} --preset=${PRESET} WORKING_DIRECTORY ${EXTERNAL_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${EXTERNAL_BINARY_DIR} --preset=${PRESET}
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE ERROR
        )

    if(NOT (${RESULT} EQUAL 0))
        message(WARNING "Output message: ${OUTPUT}")
        message(FATAL_ERROR "Error message: ${ERROR}")
    endif()
endfunction()
