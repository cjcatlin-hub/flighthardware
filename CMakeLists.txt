# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(throttles C CXX ASM)

add_compile_definitions(PICO_TINYUSB=1)
add_compile_definitions(CFG_TUSB_CONFIG_FILE="tusb_config.h")


pico_sdk_init()

add_executable(throttles
    main.c
    usb_descriptors.c
)

target_include_directories(throttles PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(throttles
    pico_stdlib
    hardware_adc
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(throttles)
