enable_language(ASM_NASM)


set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f elf")
set(TARGET_NAME "TUI_SIM-bare-metal")

add_executable(${TARGET_NAME}
        hal.c
        kbd.c
        stdfunc.c
        boot.asm
        systemInit.asm
        vectors.asm
        int10.asm
)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/inc)
# Inject the custom assert.h into the core engine libraries when building for bare metal
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc)
target_include_directories(${PROJECT_NAME}-SIM PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc)

define_filename_macro(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE
        ${PROJECT_NAME}-SIM
)
target_compile_options(${PROJECT_NAME} PRIVATE -Os -ffunction-sections -fdata-sections)
target_compile_options(${PROJECT_NAME}-SIM PRIVATE -Os -ffunction-sections -fdata-sections)
target_compile_options(${TARGET_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C>:-Os>
        $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
        $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
)

target_link_options(${TARGET_NAME} PRIVATE
        "-nostdlib"
        "-T${CMAKE_CURRENT_SOURCE_DIR}/bios.ld"
        "-Wl,-Map=mapfile.map"
        "-Wl,--gc-sections"
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ia16-elf-objcopy -O binary ${TARGET_NAME} ${TARGET_NAME}.bin
)

add_custom_target(
        run_bare-metal
        COMMAND "bochs" -q -debugger -f "${CMAKE_CURRENT_SOURCE_DIR}/bochsrc.bxrc"
        DEPENDS ${TARGET_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Launching Bochs Emulator with custom BIOS..."
)