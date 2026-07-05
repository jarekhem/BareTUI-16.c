message(STATUS "Building: Windows Native Test Environment (MinGW32)")

# ---------------------------------------------------------
# DEPENDENCY: Download pre-compiled SDL2 binaries for MinGW
# ---------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    SDL2_mingw
    URL "https://github.com/libsdl-org/SDL/releases/download/release-2.30.2/SDL2-devel-2.30.2-mingw.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP true
    SOURCE_SUBDIR ""
)
FetchContent_MakeAvailable(SDL2_mingw)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SDL2_DIR "${sdl2_mingw_SOURCE_DIR}/x86_64-w64-mingw32/lib/cmake/SDL2")
else ()
    set(SDL2_DIR "${sdl2_mingw_SOURCE_DIR}/i686-w64-mingw32/lib/cmake/SDL2")
endif ()

# Find the SDL2 package using the downloaded configuration
find_package(SDL2 REQUIRED CONFIG)

# Build executable for Windows (Tests)
add_executable(${PROJECT_NAME}-SIM-WIN hal.c)
target_link_libraries(${PROJECT_NAME}-SIM-WIN PRIVATE
        ${PROJECT_NAME}-SIM
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /W3 /Z7 /Od)
else ()
    target_compile_options(${PROJECT_NAME} PUBLIC -O0 -g -Wall -Wextra)
endif ()

# Link SDL2 static library and the main entry point wrapper
target_link_libraries(${PROJECT_NAME}-SIM-WIN PRIVATE
        SDL2::SDL2main
        SDL2::SDL2-static
        winmm
        imm32
        version
        ole32
        oleaut32
        uuid
        gdi32
        setupapi
)

add_custom_command(TARGET ${PROJECT_NAME}-SIM-WIN POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/vga-rom-fonts-ag868x16.bmp"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}-SIM-WIN>/vga-rom-fonts-ag868x16.bmp"
)
