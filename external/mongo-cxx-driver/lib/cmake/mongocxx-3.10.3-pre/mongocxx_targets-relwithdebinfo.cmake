#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mongo::mongocxx_static" for configuration "RelWithDebInfo"
set_property(TARGET mongo::mongocxx_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(mongo::mongocxx_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/mongocxx-static-rti-x64-v143-md.lib"
  )

list(APPEND _cmake_import_check_targets mongo::mongocxx_static )
list(APPEND _cmake_import_check_files_for_mongo::mongocxx_static "${_IMPORT_PREFIX}/lib/mongocxx-static-rti-x64-v143-md.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
