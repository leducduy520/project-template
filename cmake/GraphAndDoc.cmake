if(ENABLE_DOCUMENT_TARGET)
    find_package(Doxygen REQUIRED COMPONENTS dot doxygen)
    if(Doxygen_FOUND)
        set(DOCS_HTML_DIR ${DOCS_DIR}/html CACHE PATH "project html directory")
        set(DOCS_GRAPH_DIR ${DOCS_DIR}/graph CACHE PATH "project graph directory")

        add_custom_command(
            OUTPUT ${DOCS_HTML_DIR}/index.html
            COMMAND ${DOXYGEN_EXECUTABLE} ./docCfg
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
            COMMAND ${DOXYGEN_DOT_EXECUTABLE} -v -Tpng graph.dot -o ${GRAPH_OUTPUT_NAME}.png
            DEPENDS ${DOCS_GRAPH_DIR}/graph.dot
            WORKING_DIRECTORY ${DOCS_GRAPH_DIR}
            VERBATIM USES_TERMINAL
            )

        add_custom_target(
            gen_graph COMMENT "Generate project graph dependencies" DEPENDS ${DOCS_GRAPH_DIR}/${GRAPH_OUTPUT_NAME}.png
            )

        if(ENABLE_INSTALL_GRAPH_AND_DOC)
            add_custom_command(
                TARGET gen_graph
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${DOCS_DIR}/graph
                        ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                VERBATIM USES_TERMINAL
                )
            add_custom_command(
                TARGET gen_doc
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${DOCS_DIR}/html
                        ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DOCDIR}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                VERBATIM USES_TERMINAL
                )
        endif(ENABLE_INSTALL_GRAPH_AND_DOC)
    endif(Doxygen_FOUND)
endif(ENABLE_DOCUMENT_TARGET)
