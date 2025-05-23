#!/usr/bin/env python3
"""
Script to generate menu framework files and menu data header for the ESP Menu component.
Updated to look for menu.json in main/menu_user/ and output menu_data.h to components/Esp_menu/generated/.
"""

import os
import sys
import json
import jinja2
import jsonschema
from datetime import datetime

MENU_JSON_SCHEMA = {
    "type": "object",
    "properties": {
        "screens": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "name": {"type": "string"},
                    "type": {"type": "string", "enum": ["menu", "action", "submenu"]},
                    "items": {
                        "type": "array",
                        "items": {
                            "type": "object",
                            "properties": {
                                "name": {"type": "string"},
                                "type": {"type": "string", "enum": ["action", "submenu"]},
                                "callback": {"type": "string"}
                            },
                            "required": ["name", "type"],
                            "additionalProperties": False
                        }
                    }
                },
                "required": ["name", "type"],
                "additionalProperties": False
            }
        }
    },
    "required": ["screens"],
    "additionalProperties": False
}


def load_json_file(json_path):
    """
    Loads and validates the menu JSON file.

    Args:
        json_path (str): Path to the JSON file.

    Returns:
        dict: Parsed JSON data.

    Raises:
        SystemExit: If file is not found, JSON is invalid, or schema validation fails.
    """
    try:
        with open(json_path, 'r') as f:
            data = json.load(f)
        jsonschema.validate(data, MENU_JSON_SCHEMA)
        return data
    except FileNotFoundError:
        print(f"Error: JSON file {json_path} not found")
        sys.exit(1)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in {json_path}: {e}")
        sys.exit(1)
    except jsonschema.ValidationError as e:
        print(f"Error: JSON schema validation failed: {e}")
        sys.exit(1)


def generate_menu_data_h(output_path, screens):
    """
    Generates the menu_data.h header file.

    Args:
        output_path (str): Path to output the generated header.
        screens (list): List of screen definitions from the JSON.
    """
    env = jinja2.Environment(loader=jinja2.BaseLoader())
    template = """
    // Auto-generated menu data
    #ifndef MENU_DATA_H
    #define MENU_DATA_H
    void menu_init(void);
    #endif
    """
    with open(output_path, 'w') as f:
        f.write(env.from_string(template).render(screens=screens))


def generate_user_framework(menu_user_dir, screens):
    """
    Generates framework files (user_actions.h, user_actions.c, user_graphic.h) in main/menu_user/ if they don't exist.

    Args:
        menu_user_dir (str): Directory to generate framework files (main/menu_user/).
        screens (list): List of screen definitions from the JSON.
    """
    os.makedirs(menu_user_dir, exist_ok=True)

    # Extract callback names from JSON
    callbacks = set()
    for screen in screens:
        if 'items' in screen:
            for item in screen['items']:
                if 'callback' in item:
                    callbacks.add(item['callback'])

    # Define MenuParams_t for oscillator module
    menu_params_fields = [
        {"name": "frequency_pitch", "type": "uint8_t"},
        {"name": "frequency_fine", "type": "int16_t"},
        {"name": "waveform", "type": "OscWaveform_t"},
        {"name": "level", "type": "uint16_t"},
        {"name": "pulse_width", "type": "uint16_t"},
        {"name": "amp_mod_slot", "type": "uint8_t"},
        {"name": "freq_mod_slot", "type": "uint8_t"},
        {"name": "sync_source_slot", "type": "uint8_t"}
    ]

    # Generate user_actions.h
    user_actions_h_path = os.path.join(menu_user_dir, 'user_actions.h')
    if not os.path.exists(user_actions_h_path):
        env = jinja2.Environment(loader=jinja2.BaseLoader())
        template = """
        /**
         * @file user_actions.h
         * @brief Header file for user-defined menu actions and parameters for the oscillator module.
         */
        #ifndef USER_ACTIONS_H
        #define USER_ACTIONS_H
        #include "lvgl.h"
        #include "waveform_gen.h"

        /**
         * @brief Structure for oscillator module parameters.
         */
        typedef struct {
            {% for field in fields %}
            {{ field.type }} {{ field.name }}; ///< {{ field.name }} parameter
            {% endfor %}
        } MenuParams_t;

        /** @brief Global oscillator parameters. */
        extern MenuParams_t menu_params;

        /**
         * @brief Initializes project-specific state, including loading from NVS if enabled.
         */
        void user_init(void);

        /**
         * @brief Updates the LVGL display with current parameters.
         */
        void user_update_display(void);

        // Menu action callbacks
        {% for callback in callbacks %}
        /**
         * @brief {{ callback }} action callback.
         */
        void {{ callback }}(void);
        {% endfor %}

        #endif
        """
        with open(user_actions_h_path, 'w') as f:
            f.write(env.from_string(template).render(
                fields=menu_params_fields, callbacks=callbacks))
        print(f"Generated framework: {user_actions_h_path}")

    # Generate user_actions.c
    user_actions_c_path = os.path.join(menu_user_dir, 'user_actions.c')
    if not os.path.exists(user_actions_c_path):
        env = jinja2.Environment(loader=jinja2.BaseLoader())
        template = """
        /**
         * @file user_actions.c
         * @brief Implementation of user-defined menu actions for the oscillator module.
         */
        #include "user_actions.h"
        #include "Esp_menu.h"
        #ifdef CONFIG_ESPMENU_ENABLE_NVS
        #include "nvs_flash.h"
        #endif
        #include <string.h>
        #include "lvgl.h"

        /** @brief Global oscillator parameters. */
        MenuParams_t menu_params = {
            .frequency_pitch = 69,
            .frequency_fine = 0,
            .waveform = OSC_WAVE_SINE,
            .level = 65535,
            .pulse_width = 32768,
            .amp_mod_slot = 0xFF,
            .freq_mod_slot = 0xFF,
            .sync_source_slot = 0xFF
        };

        /**
         * @brief Initializes project-specific state, including loading from NVS if enabled.
         */
        void user_init(void) {
            #ifdef CONFIG_ESPMENU_ENABLE_NVS
            load_from_nvs();
            #endif
        }

        /**
         * @brief Updates the LVGL display with current parameters.
         */
        void user_update_display(void) {
            static lv_obj_t *param_label = NULL;
            if (!param_label) {
                param_label = lv_label_create(lv_scr_act());
                lv_obj_set_pos(param_label, 0, 0);
            }
            char buf[32];
            const char *wave_names[] = {"Sine", "Triangle", "Saw", "Square", "Pulse"};
            snprintf(buf, sizeof(buf), "P:%d W:%s", menu_params.frequency_pitch, wave_names[menu_params.waveform]);
            lv_label_set_text(param_label, buf);
        }

        #ifdef CONFIG_ESPMENU_ENABLE_NVS
        /**
         * @brief Saves current parameters to Non-Volatile Storage (NVS).
         */
        void save_to_nvs(void) {
            nvs_handle_t nvs;
            esp_err_t err = nvs_open("project", NVS_READWRITE, &nvs);
            if (err != ESP_OK) return;
            nvs_commit(nvs);
            nvs_close(nvs);
        }

        /**
         * @brief Loads parameters from Non-Volatile Storage (NVS).
         */
        void load_from_nvs(void) {
            nvs_handle_t nvs;
            esp_err_t err = nvs_open("project", NVS_READONLY, &nvs);
            if (err != ESP_OK) return;
            nvs_close(nvs);
            user_update_display();
        }
        #else
        /**
         * @brief Stub for saving to NVS when disabled.
         */
        void save_to_nvs(void) {}

        /**
         * @brief Stub for loading from NVS when disabled.
         */
        void load_from_nvs(void) {}
        #endif

        // Menu action callbacks
        {% for callback in callbacks %}
        /**
         * @brief {{ callback }} action callback.
         */
        void {{ callback }}(void) {
            // TODO: Implement {{ callback }} functionality
        }
        {% endfor %}
        """
        with open(user_actions_c_path, 'w') as f:
            f.write(env.from_string(template).render(callbacks=callbacks))
        print(f"Generated framework: {user_actions_c_path}")

    # Generate user_graphic.h
    user_graphic_h_path = os.path.join(menu_user_dir, 'user_graphic.h')
    if not os.path.exists(user_graphic_h_path):
        env = jinja2.Environment(loader=jinja2.BaseLoader())
        template = """
        /**
         * @file user_graphic.h
         * @brief Header file for user-defined LVGL graphics for the oscillator module.
         */
        #ifndef USER_GRAPHIC_H
        #define USER_GRAPHIC_H
        #include "lvgl.h"

        /**
         * @brief Initializes custom LVGL graphics for the project.
         * @param parent The parent LVGL object for the graphics.
         */
        void user_graphic_init(lv_obj_t *parent);

        #endif
        """
        with open(user_graphic_h_path, 'w') as f:
            f.write(env.from_string(template).render())
        print(f"Generated framework: {user_graphic_h_path}")


def main():
    """
    Main function to generate menu framework and menu_data.h.

    Args:
        sys.argv[1] (str): Path to menu.json (in main/menu_user/).
        sys.argv[2] (str): Path to output menu_data.h (in components/Esp_menu/generated/).
    """
    if len(sys.argv) != 3:
        print("Usage: generate_menu.py <json_path> <output_h_path>")
        sys.exit(1)

    json_path = sys.argv[1]
    output_h_path = sys.argv[2]

    # Determine main/menu_user/ directory
    project_dir = os.path.dirname(os.path.dirname(os.path.dirname(json_path)))
    menu_user_dir = os.path.join(project_dir, 'main', 'menu_user')

    # Load and validate JSON
    menu_data = load_json_file(json_path)

    # Generate user framework files if needed
    generate_user_framework(menu_user_dir, menu_data.get('screens', []))

    # Generate menu_data.h
    generate_menu_data_h(output_h_path, menu_data.get('screens', []))


if __name__ == "__main__":
    main()
