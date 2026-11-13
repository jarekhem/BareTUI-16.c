cmake_minimum_required(VERSION 3.19)

macro(ExecuteLocalPackageHelper)

    set(LOCAL_PACKAGE_FILE "${CMAKE_SOURCE_DIR}/local-packages.cmake")
    if (NOT EXISTS ${LOCAL_PACKAGE_FILE})
        file(WRITE ${LOCAL_PACKAGE_FILE} "macro(set_local_packages)\n\n#Example of defining local package:\n#set(CPM_PackageName_SOURCE \"/home/.../package\")\n\nendmacro()\n")
        message("Creating ${LOCAL_PACKAGE_FILE}")
    endif ()

    include(${LOCAL_PACKAGE_FILE})
    set_local_packages()

    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        string(REGEX MATCH "^CPM_(.*)_SOURCE$" ${_variableName}_MATCHED ${_variableName})
        if (${_variableName}_MATCHED)
            message(NOTICE "\t\t\tNOTICE: Using local package \"${CMAKE_MATCH_1}\" located at \"${${_variableName}}\"")
        endif()
    endforeach()

endmacro()
