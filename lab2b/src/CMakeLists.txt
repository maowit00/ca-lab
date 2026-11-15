add_executable(${PROJECT_NAME} joystick_rgb.c)

target_link_libraries(${PROJECT_NAME}
  pico_stdlib
  pico_runtime
  pico_stdio
  pico_time
  hardware_adc
  hardware_gpio
  hardware_pwm
)

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)


