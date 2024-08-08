find_package(Doxygen REQUIRED COMPONENTS dot doxygen)

if(Doxygen_FOUND)
    if(NOT DEFINED DOCS_DIR)
        set(DOCS_DIR ${CMAKE_SOURCE_DIR}/docs)
    endif(NOT DEFINED DOCS_DIR)

    message(STATUS "doc dir ${DOCS_DIR}")

    set(DOCS_HTML_DIR ${DOCS_DIR}/html CACHE PATH "project html directory")
    set(DOCS_GRAPH_DIR ${DOCS_DIR}/graph CACHE PATH "project graph directory")

    if(BUILD_GRAPH)
        add_custom_command(
            OUTPUT ${DOCS_GRAPH_DIR}/mysfmlapp-graph.png
            COMMAND ${DOXYGEN_DOT_EXECUTABLE} -v -Tpng graph.dot -o mysfmlapp-graph.png
            WORKING_DIRECTORY ${DOCS_GRAPH_DIR}
            VERBATIM USES_TERMINAL
            )

        add_custom_target(
            gen_graph COMMENT "Generate project graph dependencies"
            DEPENDS ${DOCS_GRAPH_DIR}/mysfmlapp-graph.png
            )
        set_target_properties(gen_graph PROPERTIES FOLDER "Custom target")
    endif(BUILD_GRAPH)

    if(BUILD_DOCS)
        add_custom_target(
            gen_doc
            COMMAND ${DOXYGEN_EXECUTABLE} ./docCfg
            COMMENT "Generate project document"
            WORKING_DIRECTORY ${DOCS_DIR}
            )
        set_target_properties(gen_doc PROPERTIES FOLDER "Custom target")
    endif(BUILD_DOCS)

    if(INSTALL_GRAPH)
        install(
            FILES ${DOCS_GRAPH_DIR}/mysfmlapp-graph.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/MySFMLApp
            )
    endif(INSTALL_GRAPH)

    if(INSTALL_DOCS)
        install(DIRECTORY ${DOCS_HTML_DIR} DESTINATION ${CMAKE_INSTALL_DATADIR}/MySFMLApp)
    endif(INSTALL_DOCS)
endif(Doxygen_FOUND)
