message(STATUS "Building: 16-bit Targets (ia16-elf-gcc)")

enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f elf")

add_executable(tuiDos.exe hal.c)
target_link_libraries(tuiDos.exe PRIVATE
        ${PROJECT_NAME}-SIM
)

target_link_options(tuiDos.exe PRIVATE
        "-Wl,-Map=mapfile.map"
        "-Wl,--defsym=__stack=0x1000"
)

add_custom_target(
        run_dos
        COMMAND "dosbox-x" -c "mount c ${CMAKE_CURRENT_BINARY_DIR}" -c "c:" -c "$<TARGET_FILE_NAME:tuiDos.exe>"
        DEPENDS tuiDos.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
