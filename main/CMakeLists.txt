cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(oscillator_module)
set(srcs
    "main.c"
    "waveform_gen.c"
    "menu_user/user_actions.c"
    "../components/module_i2c_proto/module_i2c_proto.c"
)
idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS
        "."
        "menu_user"
        "../components/common/include"
        "../components/module_i2c_proto/include"
        "../components/Esp_menu/include"
        "../components/Esp_menu/generated"
    PRIV_REQUIRES
        lvgl
        espressif__lvgl_port
        espressif__knob
        espressif__button
        nvs_flash
        driver
)