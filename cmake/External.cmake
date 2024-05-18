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
    set(multiValueArgs REPO TAG LABEL SHALLOW CONFIGURE_COMMAND CMAKE_AGRS BUILD_COMMAND INSTALL_COMMAND)
    set(oneValueArgs FIRST_INDX LAST_INDX)
    set(options READ_FROM_JSON SUB_PROJECT)
    cmake_parse_arguments(PKGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(PKGS_READ_FROM_JSON)
        read_deps_json(PKGS_LABEL PKGS_REPO PKGS_TAG PKGS_SHALLOW PKGS_LAST_INDX)
        set(PKGS_FIRST_INDX 0)
    endif(PKGS_READ_FROM_JSON)

    if(PKGS_SUB_PROJECT)
        set(PKGS_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAddSub.txt.in)
        #set(MODULE_STRING "include(FetchContent)\nset(FETCHCONTENT_BASE_DIR \"${CMAKE_CURRENT_BINARY_DIR}\" CACHE INTERNAL \"\")")
        set(MODULE_STRING "include(FetchContent)")
    else(PKGS_SUB_PROJECT)
        set(PKGS_ADD_FILE ${CMAKE_SOURCE_DIR}/cmake/ExternalAdd.txt.in)
        set(MODULE_STRING "include(ExternalProject)\nset_directory_properties(PROPERTIES EP_BASE ${EXTERNAL_DIR}/EP_BASE)")
    endif(PKGS_SUB_PROJECT)

    if(${PKGS_LAST_INDX} MATCHES "-1")
        message(WARNING "Last index is '-1'")
        return()
    endif()

    if(${PKGS_FIRST_INDX} MATCHES "-1")
        set(PKGS_FIRST_INDX 0)
    endif()

    # message(${PKGS_REPO})
    # message(${PKGS_TAG})
    # message(${PKGS_LABEL})
    # message(${PKGS_SHALLOW})
    # message("${PKGS_CONFIGURE_COMMAND}")
    # message("${PKGS_CMAKE_AGRS}")
    # message(${PKGS_BUILD_COMMAND})
    # message(${PKGS_INSTALL_COMMAND})
    # message(FATAL_ERROR "STOP")

    foreach(ITR RANGE ${PKGS_FIRST_INDX} ${PKGS_LAST_INDX})
        list(GET PKGS_REPO ${ITR} PKG_REPO)
        list(GET PKGS_TAG ${ITR} PKG_TAG)
        list(GET PKGS_LABEL ${ITR} PKG_LABEL)
        list(GET PKGS_SHALLOW ${ITR} PKG_SHALLOW)
        list(GET PKGS_CONFIGURE_COMMAND ${ITR} PKG_CONFIGURE_COMMAND)
        list(GET PKGS_CMAKE_AGRS ${ITR} PKG_CMAKE_AGRS)
        list(GET PKGS_BUILD_COMMAND ${ITR} PKG_BUILD_COMMAND)
        list(GET PKGS_INSTALL_COMMAND ${ITR} PKG_INSTALL_COMMAND)

        set(INIT_STRING "cmake_minimum_required(VERSION 3.27)\nproject(build-${PKG_LABEL})\n${MODULE_STRING}\n")
        file(WRITE ${EXTERNAL_DIR}/Packages/${PKG_LABEL}/CMakeLists.txt ${INIT_STRING})
        unset(INIT_STRING)

        configure_file(${PKGS_ADD_FILE} ${EXTERNAL_DIR}/Packages/${PKG_LABEL}/ExternalAdd.txt @ONLY)
        file(READ ${EXTERNAL_DIR}/Packages/${PKG_LABEL}/ExternalAdd.txt PKGS_ADD_STRING)
        file(REMOVE ${EXTERNAL_DIR}/Packages/${PKG_LABEL}/ExternalAdd.txt)

        if(NOT PKGS_SUB_PROJECT)
            message("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ${ITR}")
            string(FIND ${PKGS_ADD_STRING} ")" POS)
            string(SUBSTRING ${PKGS_ADD_STRING} 0 ${POS} PKGS_ADD_STRING)
            if(PKG_CONFIGURE_COMMAND)
                string(APPEND PKGS_ADD_STRING "\tCONFIGURE_COMMAND ${PKG_CONFIGURE_COMMAND}\n")
            endif(PKG_CONFIGURE_COMMAND)
            if(PKG_CMAKE_AGRS)
                string(APPEND PKGS_ADD_STRING "\tCMAKE_ARGS ${PKG_CMAKE_AGRS}\n")
            endif(PKG_CMAKE_AGRS)
            if(PKG_BUILD_COMMAND)
                string(APPEND PKGS_ADD_STRING "\tBUILD_COMMAND ${PKG_BUILD_COMMAND}\n")
            endif(PKG_BUILD_COMMAND)
            if(PKG_INSTALL_COMMAND)
                string(APPEND PKGS_ADD_STRING "\tINSTALL_COMMAND ${PKG_INSTALL_COMMAND}\n")
            endif(PKG_INSTALL_COMMAND)
            string(APPEND PKGS_ADD_STRING "\t)")
        endif(NOT PKGS_SUB_PROJECT)
        
        file(APPEND ${EXTERNAL_DIR}/Packages/${PKG_LABEL}/CMakeLists.txt "${PKGS_ADD_STRING}\n")

        if(NOT PKGS_SUB_PROJECT)
            configure_file(${CMAKE_SOURCE_DIR}/cmake/ExternalCMakePresets.json.in ${EXTERNAL_DIR}/Packages/${PKG_LABEL}/CMakePresets.json)

            execute_process(
                COMMAND ${CMAKE_COMMAND} --no-warn-unused-cli --preset=${PRESET} WORKING_DIRECTORY ${EXTERNAL_DIR}/Packages/${PKG_LABEL}
                )

            execute_process(
                COMMAND ${CMAKE_COMMAND} --build --preset=${PRESET}
                WORKING_DIRECTORY ${EXTERNAL_DIR}/Packages/${PKG_LABEL}
                RESULT_VARIABLE RESULT
                ERROR_VARIABLE ERROR
                )

            if(NOT (${RESULT} EQUAL 0))
                message(FATAL_ERROR "Error message: ${ERROR}")
            endif()

        else(NOT PKGS_SUB_PROJECT)
            add_subdirectory(${EXTERNAL_DIR}/Packages/${PKG_LABEL})

        endif(NOT PKGS_SUB_PROJECT)
    endforeach()
endmacro()
