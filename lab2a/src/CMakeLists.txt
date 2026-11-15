add_executable(${PROJECT_NAME} button.c)

target_link_libraries(${PROJECT_NAME} pico_runtime hardware_gpio pico_stdlib)

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

