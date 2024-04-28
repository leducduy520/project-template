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
            gen_graph
            COMMENT "Generate project graph dependencies"
            DEPENDS ${DOCS_GRAPH_DIR}/${GRAPH_OUTPUT_NAME}.png
            )

        if(ENABLE_MOVE_DOCS_TO_INSTALL)
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
        endif()
    endif()
endfunction()

function(read_deps_json)
    file(READ ${CMAKE_SOURCE_DIR}/deps.json DEPS_JSON_STRING)

    string(JSON DEPS_LENGTH LENGTH ${DEPS_JSON_STRING})
    math(EXPR LAST_INDX "${DEPS_LENGTH} - 1" OUTPUT_FORMAT DECIMAL)

    if(${LAST_INDX} STREQUAL "-1")
        message("deps.json is empty or parsed unsuccessfully")
        set(PKG_LAST_INDEX
            NO_FOUND
            PARENT_SCOPE
            )
        return()
    endif()

    set(PKG_LAST_INDEX
        ${LAST_INDX}
        PARENT_SCOPE
        )

    foreach(ITR RANGE ${LAST_INDX})
        string(JSON LABEL GET ${DEPS_JSON_STRING} ${ITR} label)
        list(APPEND LABELS ${LABEL})
        string(JSON REPO GET ${DEPS_JSON_STRING} ${ITR} repo)
        list(APPEND REPOS ${REPO})
        string(JSON TAG GET ${DEPS_JSON_STRING} ${ITR} tag)
        list(APPEND TAGS ${TAG})
        string(
            JSON
            SHALLOW
            ERROR_VARIABLE
            ERROR
            GET
            ${DEPS_JSON_STRING}
            ${ITR}
            shallow
            )
        if(ERROR)
            set(SHALLOW FALSE)
        endif()
        list(APPEND SHALLOWS ${SHALLOW})
    endforeach()

    set(PKG_TAGS
        ${TAGS}
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
    set(PKG_SHALLOWS
        ${SHALLOWS}
        PARENT_SCOPE
        )
endfunction(read_deps_json)

macro(build_external_project)
    set(multiValueArgs REPOS TAGS LABELS SHALLOWS)
    set(oneValueArgs FIRST_INDX LAST_INDX)
    set(options READ_FROM_JSON SUB_PROJECT)
    cmake_parse_arguments(EXTERNAL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(EXTERNAL_READ_FROM_JSON)
        read_deps_json()
        set(EXTERNAL_REPOS ${PKG_REPOS})
        set(EXTERNAL_TAGS ${PKG_TAGS})
        set(EXTERNAL_LABELS ${PKG_LABELS})
        set(EXTERNAL_SHALLOWS ${PKG_SHALLOWS})
        set(EXTERNAL_FIRST_INDX 0)
        set(EXTERNAL_LAST_INDX ${PKG_LAST_INDEX})
    endif(EXTERNAL_READ_FROM_JSON)

    if(EXTERNAL_SUB_PROJECT)
        set(EXTERNAL_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAddSub.txt.in)
    else(EXTERNAL_SUB_PROJECT)
        set(EXTERNAL_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAdd.txt.in)
    endif(EXTERNAL_SUB_PROJECT)

    if(${EXTERNAL_LAST_INDX} MATCHES "(-1|NO_FOUND)")
        message("Packages list's last index is not found")
        return()
    endif()

    if(${EXTERNAL_LAST_INDX} MATCHES "(-1|NO_FOUND)")
        set(EXTERNAL_FIRST_INDX 0)
    endif()

    file(WRITE ${EXTERNAL_DIR}/CMakeLists.txt
         "cmake_minimum_required(VERSION 3.27)\nproject(external)\ninclude(ExternalProject)"
         )

    foreach(ITR RANGE ${EXTERNAL_FIRST_INDX} ${EXTERNAL_LAST_INDX})
        list(GET EXTERNAL_REPOS ${ITR} EXTERNAL_REPO)
        list(GET EXTERNAL_TAGS ${ITR} EXTERNAL_BRANCH)
        list(GET EXTERNAL_LABELS ${ITR} EXTERNAL_LABEL)
        list(GET EXTERNAL_SHALLOWS ${ITR} EXTERNAL_SHALLOW)

        configure_file(${EXTERNAL_ADD_FILE} ${EXTERNAL_DIR}/ExternalAdd-${EXTERNAL_LABELS}.txt @ONLY)
        file(READ ${EXTERNAL_DIR}/ExternalAdd-${EXTERNAL_LABELS}.txt EXTERNAL_ADD_STRING)
        file(REMOVE ${EXTERNAL_DIR}/ExternalAdd-${EXTERNAL_LABELS}.txt)
        file(APPEND ${EXTERNAL_DIR}/CMakeLists.txt "\n${EXTERNAL_ADD_STRING}\n")
    endforeach()

    configure_file(${CMAKE_SOURCE_DIR}/cmake/ExternalCMakePresets.json.in ${EXTERNAL_DIR}/CMakePresets.json)

    if(NOT EXTERNAL_SUB_PROJECT)
        file(READ ${CMAKE_SOURCE_DIR}/external_cache.json EXTERNAL_CACHE_JSON)
        file(MD5 ${EXTERNAL_DIR}/CMakeLists.txt cmakelists_MD5)
        file(MD5 ${EXTERNAL_DIR}/CMakePresets.json cmakepresets_MD5)

        if(EXISTS ${EXTERNAL_BINARY_DIR}/CMakeCache.txt)
            string(JSON cmakelists_OUTPUT GET ${EXTERNAL_CACHE_JSON} cmakelists_MD5)
            string(JSON cmakepresets_OUTPUT GET ${EXTERNAL_CACHE_JSON} cmakepresets_MD5)
            if(EXTERNAL_READ_FROM_JSON)
                file(MD5 ${CMAKE_SOURCE_DIR}/deps.json deps_MD5)
                string(JSON deps_OUTPUT GET ${EXTERNAL_CACHE_JSON} deps_MD5)
                if((${deps_OUTPUT} STREQUAL ${deps_MD5})
                   AND (${cmakelists_OUTPUT} STREQUAL ${cmakelists_MD5})
                   AND (${cmakepresets_OUTPUT} STREQUAL ${cmakepresets_MD5})
                   )
                    return()
                endif()
                string(JSON EXTERNAL_CACHE_JSON SET ${EXTERNAL_CACHE_JSON} deps_MD5 "\"${deps_MD5}\"")
            endif()

        endif()

        string(JSON EXTERNAL_CACHE_JSON SET ${EXTERNAL_CACHE_JSON} cmakelists_MD5 "\"${cmakelists_MD5}\"")
        string(JSON EXTERNAL_CACHE_JSON SET ${EXTERNAL_CACHE_JSON} cmakepresets_MD5 "\"${cmakepresets_MD5}\"")
        file(WRITE ${CMAKE_SOURCE_DIR}/external_cache.json ${EXTERNAL_CACHE_JSON})
    endif(NOT EXTERNAL_SUB_PROJECT)

    execute_process(COMMAND ${CMAKE_COMMAND} --no-warn-unused-cli --preset=${PRESET} WORKING_DIRECTORY ${EXTERNAL_DIR})

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build --preset=${PRESET}
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE RESULT
        ERROR_VARIABLE ERROR
        )

    if(NOT (${RESULT} EQUAL 0))
        message(FATAL_ERROR "Error message: ${ERROR}")
    endif()

    if(EXTERNAL_SUB_PROJECT)
        foreach(ITR RANGE ${EXTERNAL_FIRST_INDX} ${EXTERNAL_LAST_INDX})
            list(GET EXTERNAL_LABELS ${ITR} EXTERNAL_LABEL)
            add_subdirectory(${EXTERNAL_DIR}/${EXTERNAL_LABEL}/src)
        endforeach()
    endif(EXTERNAL_SUB_PROJECT)

endmacro()
