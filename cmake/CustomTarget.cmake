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

function(and_external_catch2)
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDERR)
    set(RESULT)
    set(OUTPUT)
    set(ERROR)
    if(WIN32)
        set(PRESET windows)
    else()
        set(PRESET ubuntu)
    endif(WIN32)

    set(EXTERNAL_BINARY_DIR ${EXTERNAL_DIR}/build/${PRESET} PARENT_SCOPE)

    execute_process(COMMAND ${CMAKE_COMMAND} -S . -B ${EXTERNAL_DIR}/build/${PRESET} --preset=${PRESET} WORKING_DIRECTORY ${EXTERNAL_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${EXTERNAL_DIR}/build/${PRESET} --preset=${PRESET}
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
