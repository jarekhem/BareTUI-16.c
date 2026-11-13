enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -f elf")
set(TARGET_NAME "TUI_SIM-bare-metal")
set(RAM_TARGET_NAME "TUI_SIM-bare-metal-ram")

# Pull RomLoader-16 SDK package via CPM (uses local-packages.cmake when present)
include(${CMAKE_SOURCE_DIR}/cmake/get_CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/LocalPackageHelper.cmake)
ExecuteLocalPackageHelper()
CPMAddPackage("gh:jarekhem/RomLoader-16#main")

# Shared bare-metal C platform objects (compiled once, used by both ROM and RAM targets)
add_library(${TARGET_NAME}-common OBJECT
        hal.c
        kbd.c
        stdfunc.c
)

target_include_directories(${TARGET_NAME}-common PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/inc
)
target_link_libraries(${TARGET_NAME}-common PUBLIC
        RomLoader-16::SDK
        ${PROJECT_NAME}-SIM
)

# Inject the custom assert.h into the core engine libraries when building for bare metal
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc)
target_include_directories(${PROJECT_NAME}-SIM PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc)

define_filename_macro(${TARGET_NAME}-common)

target_compile_options(${PROJECT_NAME} PRIVATE -Os -ffunction-sections -fdata-sections)
target_compile_options(${PROJECT_NAME}-SIM PRIVATE -Os -ffunction-sections -fdata-sections)
target_compile_options(${TARGET_NAME}-common PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-ffreestanding>
        $<$<COMPILE_LANGUAGE:C>:-Os>
        $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
        $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
)

# ==============================================================================
# 1. Standalone 32KB ROM Image Target (TUI_SIM-bare-metal.bin)
# ==============================================================================
add_executable(${TARGET_NAME}
        boot.asm
        systemInit.asm
        vectors.asm
        int10.asm
)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/inc)
target_link_libraries(${TARGET_NAME} PRIVATE
        ${TARGET_NAME}-common
        ${PROJECT_NAME}-SIM
)

target_link_options(${TARGET_NAME} PRIVATE
        "-nostdlib"
        "-T${CMAKE_CURRENT_SOURCE_DIR}/bios.ld"
        "-Wl,-Map=${TARGET_NAME}.map"
        "-Wl,--gc-sections"
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ia16-elf-objcopy -O binary ${TARGET_NAME} ${TARGET_NAME}.bin
        COMMENT "Generating 32KB standalone ROM image (${TARGET_NAME}.bin)..."
)

add_custom_target(
        run_bare-metal
        COMMAND "bochs" -q -debugger -f "${CMAKE_CURRENT_SOURCE_DIR}/bochsrc.bxrc"
        DEPENDS ${TARGET_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Launching Bochs Emulator with custom BIOS..."
)

# ==============================================================================
# 2. Compact RAM Payload Target for RomLoader-16 (TUI_SIM-bare-metal-ram.bin)
#    Also generates:
#      - upload_TUI_SIM-bare-metal-ram
#      - run_hw_TUI_SIM-bare-metal-ram (and alias `run_hw`)
#      - run_bochs_TUI_SIM-bare-metal-ram
# ==============================================================================
add_romloader_payload(
        TARGET ${RAM_TARGET_NAME}
        LIBRARIES
            ${TARGET_NAME}-common
            ${PROJECT_NAME}-SIM
        PORT COM6
        SEG  1000
        OFF  0000
)
