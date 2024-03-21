function(add_gen_doc)
    if(NOT ENABLE_DOCUMENT_TARGET)
        return()
    endif()

    find_package(Doxygen REQUIRED)

    if(Doxygen_FOUND)
        add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/docs/html/index.html
            COMMAND doxygen ./docCfg
            DEPENDS ${CMAKE_SOURCE_DIR}/docs/docCfg
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
            VERBATIM USES_TERMINAL
            )

        add_custom_target(
            gen_doc
            COMMENT "Generate project document"
            DEPENDS ${CMAKE_SOURCE_DIR}/docs/html/index.html
            SOURCES ${CMAKE_SOURCE_DIR}/docs/docCfg
            )

        add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/docs/graph/graph.png
            COMMAND dot -v -Tpng graph.dot -o graph.png
            DEPENDS ${CMAKE_SOURCE_DIR}/docs/graph/graph.dot
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/graph
            VERBATIM USES_TERMINAL
            )

        add_custom_target(
            gen_graph
            COMMENT "Generate project graph dependencies"
            DEPENDS ${CMAKE_SOURCE_DIR}/docs/graph/graph.png
            )
    endif()
endfunction()

function(and_external_catch2)
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDERR)
    set(RESULT)
    set(OUTPUT)
    set(ERROR)
    if(WIN32)
        set(PRESET external)
    else()
        set(PRESET u-external)
    endif(WIN32)

    execute_process(COMMAND ${CMAKE_COMMAND} --preset=${PRESET} WORKING_DIRECTORY ${EXTERNAL_DIR})
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build --preset=${PRESET}
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
