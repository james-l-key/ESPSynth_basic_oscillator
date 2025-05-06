# Ensure Python dependencies are installed
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env "PATH=$ENV{PATH}"
        python3 -m pip install jinja2 jsonschema
    RESULT_VARIABLE pip_result
)
if(NOT pip_result EQUAL "0")
    message(FATAL_ERROR "Failed to install Python dependencies: jinja2, jsonschema")
endif()

# Create output directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/esp-idf/Esp_menu)

# Use an absolute path to the scripts directory
set(SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "Script directory: ${SCRIPT_DIR}")

# Check if script exists explicitly using absolute path
if(NOT EXISTS "${SCRIPT_DIR}/generate_menu.py")
    message(FATAL_ERROR "generate_menu.py not found at ${SCRIPT_DIR}/generate_menu.py")
endif()

# Make sure the script is executable
execute_process(
    COMMAND chmod +x "${SCRIPT_DIR}/generate_menu.py"
    ERROR_QUIET
)

# Check if menu.json exists
if(NOT EXISTS "${SCRIPT_DIR}/menu.json")
    message(FATAL_ERROR "menu.json not found at ${SCRIPT_DIR}/menu.json")
endif()

# Run the Python script to generate menu_data.h and menu.c
execute_process(
    COMMAND python3 "${SCRIPT_DIR}/generate_menu.py"
        "${SCRIPT_DIR}/menu.json"
        "${CMAKE_BINARY_DIR}/esp-idf/Esp_menu/menu_data.h"
    WORKING_DIRECTORY ${SCRIPT_DIR}
    RESULT_VARIABLE script_result
    OUTPUT_VARIABLE script_output
    ERROR_VARIABLE script_error
)

# Debug information if script fails
if(NOT script_result EQUAL "0")
    message(STATUS "Script failed with output: ${script_output}")
    message(STATUS "Script error: ${script_error}")
    message(FATAL_ERROR "Code generation failed")
endif()

# Include generated files
set(srcs
    "./Esp_menu.c"
    "./user_actions.c"
    "./generated/menu.c"
)

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "." "./include" "./generated" "${CMAKE_BINARY_DIR}/esp-idf/Esp_menu"
    PRIV_REQUIRES lvgl esp_lvgl_port espressif__knob espressif__button
)