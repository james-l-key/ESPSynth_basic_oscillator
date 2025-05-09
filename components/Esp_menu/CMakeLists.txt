# Install Python dependencies for menu generation
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH}"
        python3 -m pip install jinja2 jsonschema
    RESULT_VARIABLE pip_result
)
if(NOT pip_result EQUAL "0")
    message(FATAL_ERROR "Failed to install Python dependencies: jinja2, jsonschema")
endif()

# Generate menu_data.h from menu.json
execute_process(
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_menu.py
        ${CMAKE_CURRENT_SOURCE_DIR}/../../main/menu_user/menu.json
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/menu_data.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE script_result
)
if(NOT script_result EQUAL "0")
    message(FATAL_ERROR "Code generation failed")
endif()

set(srcs
    "Esp_menu.c"
    "generated/menu.c"
)
set(requires
    lvgl
    espressif__lvgl_port
    espressif__knob
    espressif__button
)
if(CONFIG_ESPMENU_ENABLE_NVS)
    list(APPEND requires nvs_flash)
endif()
idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "." "include" "generated" "../../main" "../../main/menu_user"
    PRIV_REQUIRES "${requires}"
)