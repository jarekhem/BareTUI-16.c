message(STATUS "Building: 16-bit Targets (ia16-elf-gcc)")

enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f elf")

add_executable(tuiDos.exe hal.c)

target_include_directories(tuiDos.exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bare-metal/inc)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bare-metal/inc)
target_include_directories(${PROJECT_NAME}-SIM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../bare-metal/inc)

define_filename_macro(tuiDos.exe)

target_link_libraries(tuiDos.exe PRIVATE
        ${PROJECT_NAME}-SIM
)

target_compile_options(${PROJECT_NAME} PRIVATE -Os -ffunction-sections -fdata-sections)
target_compile_options(${PROJECT_NAME}-SIM PRIVATE -Os -ffunction-sections -fdata-sections)
target_compile_options(tuiDos.exe PRIVATE -Os -ffunction-sections -fdata-sections)

target_link_options(tuiDos.exe PRIVATE
        "-Wl,-Map=mapfile.map"
        "-Wl,--defsym=__stack=0x1000"
        "-Wl,--gc-sections"
)

add_custom_target(
        run_dos
        COMMAND "dosbox-x" -c "mount c ${CMAKE_CURRENT_BINARY_DIR}" -c "c:" -c "$<TARGET_FILE_NAME:tuiDos.exe>"
        DEPENDS tuiDos.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
