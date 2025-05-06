#include "menu_data.h"
#include "lvgl.h"
#include "espressif__lvgl_port.h"
#include "espressif__knob.h"
#include "espressif__button.h"
#include "string.h"


#include "user_graphic.h"


static void event_handler(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        const char *text = lv_list_get_btn_text(obj);
        
        
        /* DEBUG: Widget type=list, items=[{'text': 'Pitch Up', 'action': 'pitch_up'}, {'text': 'Pitch Down', 'action': 'pitch_down'}, {'text': 'Waveform', 'screen': 'waveform'}, {'text': 'Level/Fine', 'screen': 'level_fine'}, {'text': 'Pulse Width/AmpMod', 'screen': 'pw_ampmod'}, {'text': 'Favorites', 'screen': 'favorites'}] */
        
        /* DEBUG: Processing list widget with 6 items */
        
        /* DEBUG: Adding item Pitch Up */
        if (strcmp(text, "Pitch Up") == 0) {
            
            pitch_up();
            
        }
        
        /* DEBUG: Adding item Pitch Down */
        if (strcmp(text, "Pitch Down") == 0) {
            
            pitch_down();
            
        }
        
        /* DEBUG: Adding item Waveform */
        if (strcmp(text, "Waveform") == 0) {
            
            lv_scr_load(scr_waveform);
            
        }
        
        /* DEBUG: Adding item Level/Fine */
        if (strcmp(text, "Level/Fine") == 0) {
            
            lv_scr_load(scr_level_fine);
            
        }
        
        /* DEBUG: Adding item Pulse Width/AmpMod */
        if (strcmp(text, "Pulse Width/AmpMod") == 0) {
            
            lv_scr_load(scr_pw_ampmod);
            
        }
        
        /* DEBUG: Adding item Favorites */
        if (strcmp(text, "Favorites") == 0) {
            
            lv_scr_load(scr_favorites);
            
        }
        
        
        
        /* DEBUG: Widget type=image, items=<built-in method items of dict object at 0x7fa8c2b780> */
        
        /* DEBUG: Skipped widget type=image, items=<built-in method items of dict object at 0x7fa8c2b780> */
        
        
        
        
        /* DEBUG: Widget type=list, items=[{'text': 'Next Waveform', 'action': 'waveform_next'}, {'text': 'Previous Waveform', 'action': 'waveform_prev'}, {'text': 'Back', 'screen': 'main'}] */
        
        /* DEBUG: Processing list widget with 3 items */
        
        /* DEBUG: Adding item Next Waveform */
        if (strcmp(text, "Next Waveform") == 0) {
            
            waveform_next();
            
        }
        
        /* DEBUG: Adding item Previous Waveform */
        if (strcmp(text, "Previous Waveform") == 0) {
            
            waveform_prev();
            
        }
        
        /* DEBUG: Adding item Back */
        if (strcmp(text, "Back") == 0) {
            
            lv_scr_load(scr_main);
            
        }
        
        
        
        
        
        /* DEBUG: Widget type=list, items=[{'text': 'Level Up', 'action': 'level_up'}, {'text': 'Level Down', 'action': 'level_down'}, {'text': 'Fine Tune Up', 'action': 'fine_tune_up'}, {'text': 'Fine Tune Down', 'action': 'fine_tune_down'}, {'text': 'Back', 'screen': 'main'}] */
        
        /* DEBUG: Processing list widget with 5 items */
        
        /* DEBUG: Adding item Level Up */
        if (strcmp(text, "Level Up") == 0) {
            
            level_up();
            
        }
        
        /* DEBUG: Adding item Level Down */
        if (strcmp(text, "Level Down") == 0) {
            
            level_down();
            
        }
        
        /* DEBUG: Adding item Fine Tune Up */
        if (strcmp(text, "Fine Tune Up") == 0) {
            
            fine_tune_up();
            
        }
        
        /* DEBUG: Adding item Fine Tune Down */
        if (strcmp(text, "Fine Tune Down") == 0) {
            
            fine_tune_down();
            
        }
        
        /* DEBUG: Adding item Back */
        if (strcmp(text, "Back") == 0) {
            
            lv_scr_load(scr_main);
            
        }
        
        
        
        
        
        /* DEBUG: Widget type=list, items=[{'text': 'Pulse Width Up', 'action': 'pulse_width_up'}, {'text': 'Pulse Width Down', 'action': 'pulse_width_down'}, {'text': 'Amp Mod Slot Next', 'action': 'amp_mod_slot_next'}, {'text': 'Amp Mod Slot Prev', 'action': 'amp_mod_slot_prev'}, {'text': 'Back', 'screen': 'main'}] */
        
        /* DEBUG: Processing list widget with 5 items */
        
        /* DEBUG: Adding item Pulse Width Up */
        if (strcmp(text, "Pulse Width Up") == 0) {
            
            pulse_width_up();
            
        }
        
        /* DEBUG: Adding item Pulse Width Down */
        if (strcmp(text, "Pulse Width Down") == 0) {
            
            pulse_width_down();
            
        }
        
        /* DEBUG: Adding item Amp Mod Slot Next */
        if (strcmp(text, "Amp Mod Slot Next") == 0) {
            
            amp_mod_slot_next();
            
        }
        
        /* DEBUG: Adding item Amp Mod Slot Prev */
        if (strcmp(text, "Amp Mod Slot Prev") == 0) {
            
            amp_mod_slot_prev();
            
        }
        
        /* DEBUG: Adding item Back */
        if (strcmp(text, "Back") == 0) {
            
            lv_scr_load(scr_main);
            
        }
        
        
        
        
        
        /* DEBUG: Widget type=list, items=[{'text': 'Next Slot', 'action': 'select_favorite_slot_next'}, {'text': 'Previous Slot', 'action': 'select_favorite_slot_prev'}, {'text': 'Save Favorite', 'action': 'save_favorite_action'}, {'text': 'Load Favorite', 'action': 'load_favorite_action'}, {'text': 'Clear Favorite', 'action': 'clear_favorite_action'}, {'text': 'Back', 'screen': 'main'}] */
        
        /* DEBUG: Processing list widget with 6 items */
        
        /* DEBUG: Adding item Next Slot */
        if (strcmp(text, "Next Slot") == 0) {
            
            select_favorite_slot_next();
            
        }
        
        /* DEBUG: Adding item Previous Slot */
        if (strcmp(text, "Previous Slot") == 0) {
            
            select_favorite_slot_prev();
            
        }
        
        /* DEBUG: Adding item Save Favorite */
        if (strcmp(text, "Save Favorite") == 0) {
            
            save_favorite_action();
            
        }
        
        /* DEBUG: Adding item Load Favorite */
        if (strcmp(text, "Load Favorite") == 0) {
            
            load_favorite_action();
            
        }
        
        /* DEBUG: Adding item Clear Favorite */
        if (strcmp(text, "Clear Favorite") == 0) {
            
            clear_favorite_action();
            
        }
        
        /* DEBUG: Adding item Back */
        if (strcmp(text, "Back") == 0) {
            
            lv_scr_load(scr_main);
            
        }
        
        
        
        
    }
}

void menu_init(void) {
    // Display setup
    lvgl_port_cfg_t lvgl_cfg = {
        .disp_cfg = {
            .type = LVGL_PORT_DISP_TYPE_I2C,
            .width = 128,
            .height = 64,
            .i2c = {
                .sda = 18,
                .scl = 19,
                .addr = 0x3C
            }
        }
    };
    lvgl_port_init(&lvgl_cfg);
    lv_disp_t *disp = lvgl_port_add_disp(&lvgl_cfg.disp_cfg);

    // Encoder setup
    
    knob_handle_t encoder1 = knob_create(10, 11);
    button_handle_t encoder1_btn = button_create(12);
    lv_indev_t *indev_encoder1 = lvgl_port_add_indev(encoder1, encoder1_btn);
    lv_group_t *group_encoder1 = lv_group_create();
    
    knob_handle_t encoder2 = knob_create(13, 14);
    button_handle_t encoder2_btn = button_create(15);
    lv_indev_t *indev_encoder2 = lvgl_port_add_indev(encoder2, encoder2_btn);
    lv_group_t *group_encoder2 = lv_group_create();
    
    knob_handle_t encoder3 = knob_create(16, 17);
    button_handle_t encoder3_btn = button_create(21);
    lv_indev_t *indev_encoder3 = lvgl_port_add_indev(encoder3, encoder3_btn);
    lv_group_t *group_encoder3 = lv_group_create();
    
    knob_handle_t encoder4 = knob_create(22, 23);
    button_handle_t encoder4_btn = button_create(25);
    lv_indev_t *indev_encoder4 = lvgl_port_add_indev(encoder4, encoder4_btn);
    lv_group_t *group_encoder4 = lv_group_create();
    

    // Menu setup
    
    lv_obj_t *scr_main = lv_obj_create(NULL);
    
    /* DEBUG: Widget type=list, items=[{'text': 'Pitch Up', 'action': 'pitch_up'}, {'text': 'Pitch Down', 'action': 'pitch_down'}, {'text': 'Waveform', 'screen': 'waveform'}, {'text': 'Level/Fine', 'screen': 'level_fine'}, {'text': 'Pulse Width/AmpMod', 'screen': 'pw_ampmod'}, {'text': 'Favorites', 'screen': 'favorites'}] */
    
    /* DEBUG: Processing list widget with 6 items */
    lv_obj_t *list_main_1 = lv_list_create(scr_main);
    
    /* DEBUG: Adding item Pitch Up */
    lv_list_add_btn(list_main_1, NULL, "Pitch Up");
    
    /* DEBUG: Adding item Pitch Down */
    lv_list_add_btn(list_main_2, NULL, "Pitch Down");
    
    /* DEBUG: Adding item Waveform */
    lv_list_add_btn(list_main_3, NULL, "Waveform");
    
    /* DEBUG: Adding item Level/Fine */
    lv_list_add_btn(list_main_4, NULL, "Level/Fine");
    
    /* DEBUG: Adding item Pulse Width/AmpMod */
    lv_list_add_btn(list_main_5, NULL, "Pulse Width/AmpMod");
    
    /* DEBUG: Adding item Favorites */
    lv_list_add_btn(list_main_6, NULL, "Favorites");
    
    lv_obj_add_event_cb(list_main_1, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group_encoder1, list_main_1);
    lv_indev_set_group(indev_encoder1, group_encoder1);
    
    
    /* DEBUG: Widget type=image, items=<built-in method items of dict object at 0x7fa8c2b780> */
    
    lv_obj_t *img_main_2 = lv_image_create(scr_main);
    lv_image_set_src(img_main_2, &waveform_icon_dsc);
    lv_obj_set_pos(img_main_2, 100, 0);
    
    
    
    lv_obj_t *scr_waveform = lv_obj_create(NULL);
    
    /* DEBUG: Widget type=list, items=[{'text': 'Next Waveform', 'action': 'waveform_next'}, {'text': 'Previous Waveform', 'action': 'waveform_prev'}, {'text': 'Back', 'screen': 'main'}] */
    
    /* DEBUG: Processing list widget with 3 items */
    lv_obj_t *list_waveform_1 = lv_list_create(scr_waveform);
    
    /* DEBUG: Adding item Next Waveform */
    lv_list_add_btn(list_waveform_1, NULL, "Next Waveform");
    
    /* DEBUG: Adding item Previous Waveform */
    lv_list_add_btn(list_waveform_2, NULL, "Previous Waveform");
    
    /* DEBUG: Adding item Back */
    lv_list_add_btn(list_waveform_3, NULL, "Back");
    
    lv_obj_add_event_cb(list_waveform_1, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group_encoder2, list_waveform_1);
    lv_indev_set_group(indev_encoder2, group_encoder2);
    
    
    
    lv_obj_t *scr_level_fine = lv_obj_create(NULL);
    
    /* DEBUG: Widget type=list, items=[{'text': 'Level Up', 'action': 'level_up'}, {'text': 'Level Down', 'action': 'level_down'}, {'text': 'Fine Tune Up', 'action': 'fine_tune_up'}, {'text': 'Fine Tune Down', 'action': 'fine_tune_down'}, {'text': 'Back', 'screen': 'main'}] */
    
    /* DEBUG: Processing list widget with 5 items */
    lv_obj_t *list_level_fine_1 = lv_list_create(scr_level_fine);
    
    /* DEBUG: Adding item Level Up */
    lv_list_add_btn(list_level_fine_1, NULL, "Level Up");
    
    /* DEBUG: Adding item Level Down */
    lv_list_add_btn(list_level_fine_2, NULL, "Level Down");
    
    /* DEBUG: Adding item Fine Tune Up */
    lv_list_add_btn(list_level_fine_3, NULL, "Fine Tune Up");
    
    /* DEBUG: Adding item Fine Tune Down */
    lv_list_add_btn(list_level_fine_4, NULL, "Fine Tune Down");
    
    /* DEBUG: Adding item Back */
    lv_list_add_btn(list_level_fine_5, NULL, "Back");
    
    lv_obj_add_event_cb(list_level_fine_1, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group_encoder3, list_level_fine_1);
    lv_indev_set_group(indev_encoder3, group_encoder3);
    
    
    
    lv_obj_t *scr_pw_ampmod = lv_obj_create(NULL);
    
    /* DEBUG: Widget type=list, items=[{'text': 'Pulse Width Up', 'action': 'pulse_width_up'}, {'text': 'Pulse Width Down', 'action': 'pulse_width_down'}, {'text': 'Amp Mod Slot Next', 'action': 'amp_mod_slot_next'}, {'text': 'Amp Mod Slot Prev', 'action': 'amp_mod_slot_prev'}, {'text': 'Back', 'screen': 'main'}] */
    
    /* DEBUG: Processing list widget with 5 items */
    lv_obj_t *list_pw_ampmod_1 = lv_list_create(scr_pw_ampmod);
    
    /* DEBUG: Adding item Pulse Width Up */
    lv_list_add_btn(list_pw_ampmod_1, NULL, "Pulse Width Up");
    
    /* DEBUG: Adding item Pulse Width Down */
    lv_list_add_btn(list_pw_ampmod_2, NULL, "Pulse Width Down");
    
    /* DEBUG: Adding item Amp Mod Slot Next */
    lv_list_add_btn(list_pw_ampmod_3, NULL, "Amp Mod Slot Next");
    
    /* DEBUG: Adding item Amp Mod Slot Prev */
    lv_list_add_btn(list_pw_ampmod_4, NULL, "Amp Mod Slot Prev");
    
    /* DEBUG: Adding item Back */
    lv_list_add_btn(list_pw_ampmod_5, NULL, "Back");
    
    lv_obj_add_event_cb(list_pw_ampmod_1, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group_encoder4, list_pw_ampmod_1);
    lv_indev_set_group(indev_encoder4, group_encoder4);
    
    
    
    lv_obj_t *scr_favorites = lv_obj_create(NULL);
    
    /* DEBUG: Widget type=list, items=[{'text': 'Next Slot', 'action': 'select_favorite_slot_next'}, {'text': 'Previous Slot', 'action': 'select_favorite_slot_prev'}, {'text': 'Save Favorite', 'action': 'save_favorite_action'}, {'text': 'Load Favorite', 'action': 'load_favorite_action'}, {'text': 'Clear Favorite', 'action': 'clear_favorite_action'}, {'text': 'Back', 'screen': 'main'}] */
    
    /* DEBUG: Processing list widget with 6 items */
    lv_obj_t *list_favorites_1 = lv_list_create(scr_favorites);
    
    /* DEBUG: Adding item Next Slot */
    lv_list_add_btn(list_favorites_1, NULL, "Next Slot");
    
    /* DEBUG: Adding item Previous Slot */
    lv_list_add_btn(list_favorites_2, NULL, "Previous Slot");
    
    /* DEBUG: Adding item Save Favorite */
    lv_list_add_btn(list_favorites_3, NULL, "Save Favorite");
    
    /* DEBUG: Adding item Load Favorite */
    lv_list_add_btn(list_favorites_4, NULL, "Load Favorite");
    
    /* DEBUG: Adding item Clear Favorite */
    lv_list_add_btn(list_favorites_5, NULL, "Clear Favorite");
    
    /* DEBUG: Adding item Back */
    lv_list_add_btn(list_favorites_6, NULL, "Back");
    
    lv_obj_add_event_cb(list_favorites_1, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group_encoder1, list_favorites_1);
    lv_indev_set_group(indev_encoder1, group_encoder1);
    
    
    

    // Load initial screen
    lv_scr_load(scr_main);
}