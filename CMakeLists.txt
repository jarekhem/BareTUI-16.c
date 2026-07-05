cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME BareTUI-16-C)
project(${PROJECT_NAME} C)

# Core files
add_library(${PROJECT_NAME} OBJECT
        src/Widget.c
        src/Screen.c
        src/Engine.c
)

# Widgets
target_sources(${PROJECT_NAME} PRIVATE
        src/Widgets/Label.c
        src/Widgets/Checkbox.c
        src/Widgets/RadioButton.c

        src/Widgets/VLine.c
        src/Widgets/HLine.c
        src/Widgets/Button.c
)

# Containers
target_sources(${PROJECT_NAME} PRIVATE
        src/Containers/Container.c
        src/Containers/Panel.c
        src/Containers/ColumnGroup.c
        src/Containers/ViewFlipper.c
        src/Containers/TabContainer.c
)

target_include_directories(${PROJECT_NAME} PUBLIC inc)

function(define_filename_macro target)
    get_target_property(sources ${target} SOURCES)
    if (sources)
        foreach(src IN LISTS sources)
            get_filename_component(basename ${src} NAME)
            set_property(SOURCE ${src} APPEND PROPERTY COMPILE_DEFINITIONS "__FILENAME__=\"${basename}\"")
        endforeach()
    endif()
endfunction()

define_filename_macro(${PROJECT_NAME})

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(Simulator)
endif ()

