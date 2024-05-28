function(read_deps_json IPKGS_LABEL IPKGS_REPO IPKGS_TAG IPKGS_SHALLOW IPKGS_LAST_INDEX)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/deps.json")
        message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/deps.json is not existed")
        return()
    endif(NOT EXISTS "${CMAKE_SOURCE_DIR}/deps.json")

    file(READ ${CMAKE_SOURCE_DIR}/deps.json DEPS_JSON_STRING)

    string(JSON JSON_SIZE LENGTH ${DEPS_JSON_STRING})
    math(EXPR ${IPKGS_LAST_INDEX} "${JSON_SIZE} - 1" OUTPUT_FORMAT DECIMAL)

    if(${${IPKGS_LAST_INDEX}} STREQUAL "-1")
        message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/deps.json is empty or wrong format")
        return()
    endif()

    foreach(ITR RANGE ${${IPKGS_LAST_INDEX}})
        string(JSON LABEL GET ${DEPS_JSON_STRING} ${ITR} label)
        string(JSON REPO GET ${DEPS_JSON_STRING} ${ITR} repo)
        string(JSON TAG GET ${DEPS_JSON_STRING} ${ITR} tag)
        string(JSON SHALLOW GET ${DEPS_JSON_STRING} ${ITR} shallow)

        list(APPEND ${IPKGS_LABEL} ${LABEL})
        list(APPEND ${IPKGS_REPO} ${REPO})
        list(APPEND ${IPKGS_TAG} ${TAG})
        list(APPEND ${IPKGS_SHALLOW} ${SHALLOW})
    endforeach()

    set(${IPKGS_LABEL} ${${IPKGS_LABEL}} PARENT_SCOPE)
    set(${IPKGS_REPO} ${${IPKGS_REPO}} PARENT_SCOPE)
    set(${IPKGS_TAG} ${${IPKGS_TAG}} PARENT_SCOPE)
    set(${IPKGS_SHALLOW} ${${IPKGS_SHALLOW}} PARENT_SCOPE)
    set(${IPKGS_LAST_INDEX} ${${IPKGS_LAST_INDEX}} PARENT_SCOPE)
endfunction(read_deps_json)

macro(build_external_project)
    set(multiValueArgs
        REPO
        TAG
        LABEL
        SHALLOW
        CONFIGURE_COMMAND
        CMAKE_ARGS
        CMAKE_ARGS_MORE
        BUILD_COMMAND
        INSTALL_COMMAND
        )
    set(oneValueArgs FIRST_INDX LAST_INDX)
    set(options READ_FROM_JSON SUB_PROJECT)
    cmake_parse_arguments(PKGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(PKGS_READ_FROM_JSON)
        read_deps_json(PKGS_LABEL PKGS_REPO PKGS_TAG PKGS_SHALLOW PKGS_LAST_INDX)
        set(PKGS_FIRST_INDX 0)

        if(PKGS_LAST_INDX LESS 0 OR PKGS_LAST_INDX LESS ${PKGS_FIRST_INDX})
            message(FATAL_ERROR "Last index is '-1'")
            return()
        endif()
    else()
        if(PKGS_FIRST_INDX LESS 0 OR "${PKGS_FIRST_INDX}" STREQUAL "")
            set(PKGS_FIRST_INDX 0)
        endif()

        if(PKGS_LAST_INDX LESS 0 OR PKGS_LAST_INDX LESS ${PKGS_FIRST_INDX} OR "${PKGS_LAST_INDX}" STREQUAL "")
            list(LENGTH PKGS_REPO PKGS_LAST_INDX)
            if(PKGS_LAST_INDX LESS 0 OR PKGS_LAST_INDX LESS ${PKGS_FIRST_INDX})
                set(PKGS_LAST_INDX ${PKGS_FIRST_INDX})
            else()
                math(EXPR PKGS_LAST_INDX "${PKGS_LAST_INDX} - 1" OUTPUT_FORMAT DECIMAL)
            endif()
        endif()
    endif(PKGS_READ_FROM_JSON)

    if(PKGS_SUB_PROJECT)
        set(PKGS_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAddSub.txt.in)
        set(MODULE_STRING "cmake_minimum_required(VERSION 3.25)\nproject(subproject)\ninclude(FetchContent)\n")
        set(CMAKE_LISTS_FILE_DIR ${EXTERNAL_DIR}/Packages/sub-project)
        file(MAKE_DIRECTORY ${EXTERNAL_DIR}/Packages/sub-project)
    else(PKGS_SUB_PROJECT)
        set(PKGS_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAdd.txt.in)
        set(MODULE_STRING
            "cmake_minimum_required(VERSION 3.25)\nproject(subproject)\ninclude(ExternalProject)\nset_directory_properties(PROPERTIES EP_BASE ${EXTERNAL_DIR}/EP_BASE)\n"
            )
        set(CMAKE_LISTS_FILE_DIR ${EXTERNAL_DIR}/Packages/non-sub-project)
        file(MAKE_DIRECTORY ${EXTERNAL_DIR}/Packages/non-sub-project)
    endif(PKGS_SUB_PROJECT)

    file(TOUCH ${CMAKE_LISTS_FILE_DIR}/CMakeLists.txt)
    if(NOT SUBPROJECT_FIRST_TOUCH AND PKGS_SUB_PROJECT)
        file(WRITE ${CMAKE_LISTS_FILE_DIR}/CMakeLists.txt "${MODULE_STRING}")
        set(SUBPROJECT_FIRST_TOUCH ON)
    endif(NOT SUBPROJECT_FIRST_TOUCH AND PKGS_SUB_PROJECT)

    if(NOT NONSUBPROJECT_FIRST_TOUCH AND NOT PKGS_SUB_PROJECT)
        file(WRITE ${CMAKE_LISTS_FILE_DIR}/CMakeLists.txt "${MODULE_STRING}")
        set(NONSUBPROJECT_FIRST_TOUCH ON)
    endif(NOT NONSUBPROJECT_FIRST_TOUCH AND NOT PKGS_SUB_PROJECT)

    foreach(ITR RANGE ${PKGS_FIRST_INDX} ${PKGS_LAST_INDX})
        list(GET PKGS_REPO ${ITR} PKG_REPO)
        list(GET PKGS_TAG ${ITR} PKG_TAG)
        list(GET PKGS_LABEL ${ITR} PKG_LABEL)
        list(GET PKGS_SHALLOW ${ITR} PKG_SHALLOW)
        list(GET PKGS_CONFIGURE_COMMAND ${ITR} PKG_CONFIGURE_COMMAND)
        list(GET PKGS_CMAKE_ARGS ${ITR} PKG_CMAKE_ARGS)
        list(GET PKGS_CMAKE_ARGS_MORE ${ITR} PKG_CMAKE_ARGS_MORE)
        list(GET PKGS_BUILD_COMMAND ${ITR} PKG_BUILD_COMMAND)
        list(GET PKGS_INSTALL_COMMAND ${ITR} PKG_INSTALL_COMMAND)

        configure_file(${PKGS_ADD_FILE} ${CMAKE_LISTS_FILE_DIR}/${PKG_LABEL}/CMakeLists.txt @ONLY)
        if(NOT PKGS_SUB_PROJECT)
            file(READ ${CMAKE_LISTS_FILE_DIR}/${PKG_LABEL}/CMakeLists.txt PKGS_ADD_STRING)
            string(FIND ${PKGS_ADD_STRING} ")" POS REVERSE)
            string(SUBSTRING ${PKGS_ADD_STRING} 0 ${POS} PKGS_ADD_STRING)

            if(PKG_CONFIGURE_COMMAND)
                string(APPEND PKGS_ADD_STRING "\tCONFIGURE_COMMAND ${PKG_CONFIGURE_COMMAND}\n")
            endif(PKG_CONFIGURE_COMMAND)

            if(PKG_CMAKE_ARGS)
                string(APPEND PKGS_ADD_STRING "\tCMAKE_ARGS ${PKG_CMAKE_ARGS} ${PKG_CMAKE_ARGS_MORE}\n")
            else(PKG_CMAKE_ARGS)
                string(
                    APPEND
                    PKGS_ADD_STRING
                    "\tCMAKE_ARGS \"-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>\" \"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}\" \"-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}\" \"-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}\"  ${PKG_CMAKE_ARGS_MORE}\n"
                    )
                list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_DIR}/EP_BASE/Install/${PKG_LABEL}")
            endif(PKG_CMAKE_ARGS)

            if(PKG_BUILD_COMMAND)
                string(APPEND PKGS_ADD_STRING "\tBUILD_COMMAND ${PKG_BUILD_COMMAND}\n")
            endif(PKG_BUILD_COMMAND)

            if(PKG_INSTALL_COMMAND)
                string(APPEND PKGS_ADD_STRING "\tINSTALL_COMMAND ${PKG_INSTALL_COMMAND}\n")
            endif(PKG_INSTALL_COMMAND)

            string(APPEND PKGS_ADD_STRING "\t)")
            file(WRITE ${CMAKE_LISTS_FILE_DIR}/${PKG_LABEL}/CMakeLists.txt "${PKGS_ADD_STRING}")
        endif()
        file(READ ${CMAKE_LISTS_FILE_DIR}/CMakeLists.txt MAIN_ADD_STRING)
        string(FIND "${MAIN_ADD_STRING}" "add_subdirectory(${PKG_LABEL})" POS)
        if(POS EQUAL -1)
            file(APPEND ${CMAKE_LISTS_FILE_DIR}/CMakeLists.txt "add_subdirectory(${PKG_LABEL})\n")
        endif(POS EQUAL -1)

    endforeach()

    if(PKGS_SUB_PROJECT)
        add_subdirectory(${CMAKE_LISTS_FILE_DIR})
    else(PKGS_SUB_PROJECT)
        execute_process(
            COMMAND
                ${CMAKE_COMMAND} --no-warn-unused-cli -G ${CMAKE_GENERATOR} -B ./build -S .
                -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            WORKING_DIRECTORY ${CMAKE_LISTS_FILE_DIR}
            RESULT_VARIABLE RESULT
            ERROR_VARIABLE ERROR
            )

        if(NOT (${RESULT} EQUAL 0))
            message(FATAL_ERROR "Error message: ${ERROR}")
        endif()

        execute_process(
            COMMAND ${CMAKE_COMMAND} --build ./build
            WORKING_DIRECTORY ${CMAKE_LISTS_FILE_DIR}
            RESULT_VARIABLE RESULT
            ERROR_VARIABLE ERROR
            )

        if(NOT (${RESULT} EQUAL 0))
            message(FATAL_ERROR "Error message: ${ERROR}")
        endif()
    endif(PKGS_SUB_PROJECT)
endmacro()
