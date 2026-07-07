// Copyright 2023-2024 Juraj Fiala (@jurf)
// Copyright 2025 Dasky (@daskygit)
// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <string.h>
#include "quantum.h"
#include "state/split_state.h"
#include "display/oled_master.h"
#include "display/oled_slave.h"
#include "display/oledWidget/system_widget.h"
#include "display/oledWidget/keylogger_widget.h"
#include "keymap.h"
#include "features/encoder.h"

extern uint16_t loop_rate;
static uint16_t current_keycode = 0xFF;

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return rotation;
}

bool process_detected_host_os_kb(os_variant_t detected_os) {
    if (!process_detected_host_os_user(detected_os)) {
        return false;
    }
    set_system_os(detected_os);
    return true;
}

void keyboard_post_init_kb(void) {
    split_state_init();
    keyboard_post_init_user();
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    state = layer_state_set_user(state);
    return state;
}

layer_state_t default_layer_state_set_kb(layer_state_t state) {
    state = default_layer_state_set_user(state);
    return state;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    current_keycode = keycode;
    return process_record_user(keycode, record);
}

uint16_t loop_rate = 0;
void     housekeeping_task_kb(void) {
    split_state_housekeeping();
    if (is_keyboard_master()) {
        static uint32_t     loop_count = 0;
        static fast_timer_t loop_time  = 0;
        loop_count++;
        if (timer_elapsed_fast(loop_time) > 1000) {
            loop_time  = timer_read_fast();
            loop_rate  = loop_count > UINT16_MAX ? UINT16_MAX : loop_count;
            loop_count = 0;
        }
    }
}

void oled_reinit_slave(void) {
    oled_init(OLED_ROTATION_270);
    oled_clear();
}

static void oled_write_padded_5(const char *str) {
    char buf[6] = "     ";
    int  len    = strlen(str);
    if (len > 5)
        len = 5;
    memcpy(buf, str, len);
    oled_write(buf, false);
}

void render_master(uint16_t current_keycode, uint16_t loop_rate) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);

    // Row 0: Layer header
    oled_set_cursor(0, 0);
    oled_write("Layer", false);

    // Row 1: Line separator
    oled_set_cursor(0, 1);
    oled_write("-----", false);

    // Row 2: Layer name
    oled_set_cursor(0, 2);
    oled_write_padded_5(layer_string(layer));

    // Row 3: Blank/Spacer
    oled_set_cursor(0, 3);
    oled_write("     ", false);

    // Row 4: Key header
    oled_set_cursor(0, 4);
    oled_write("Key  ", false);

    // Row 5: Line separator
    oled_set_cursor(0, 5);
    oled_write("-----", false);

    // Row 6: Last key pressed
    oled_set_cursor(0, 6);
    const char *key_str = (current_keycode == 0xFF) ? "None" : keycode_string(current_keycode);
    oled_write_padded_5(key_str);

    // Row 7: Blank/Spacer
    oled_set_cursor(0, 7);
    oled_write("     ", false);

    // Row 8: Mods header
    oled_set_cursor(0, 8);
    oled_write("Mods ", false);

    // Row 9: Line separator
    oled_set_cursor(0, 9);
    oled_write("-----", false);

    // Row 10: Active mods
    oled_set_cursor(0, 10);
    uint8_t mods        = get_mods();
    char    mods_str[6] = ".....";
    if (mods & MOD_MASK_GUI)
        mods_str[0] = 'G';
    if (mods & MOD_MASK_ALT)
        mods_str[1] = 'A';
    if (mods & MOD_MASK_CTRL)
        mods_str[2] = 'C';
    if (mods & MOD_MASK_SHIFT)
        mods_str[3] = 'S';
    mods_str[4] = ' ';
    mods_str[5] = '\0';
    oled_write(mods_str, false);

    // Row 11: Blank/Spacer
    oled_set_cursor(0, 11);
    oled_write("     ", false);

    // Row 12: Enc header
    oled_set_cursor(0, 12);
    oled_write("Enc  ", false);

    // Row 13: Line separator
    oled_set_cursor(0, 13);
    oled_write("-----", false);

    // Row 14: Left Encoder mode
    oled_set_cursor(0, 14);
    switch (enc0_mode) {
        case ENC0_MODE_UNDOREDO:
            oled_write("U/R", false);
            break;
        case ENC0_MODE_VOLUME:
            oled_write("Vol", false);
            break;
        default:
            oled_write("???", false);
    }

    // Row 15: Blank
    oled_set_cursor(0, 15);
    oled_write("     ", false);
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }

    if (!is_oled_active()) {
        oled_off();
        return false;
    }
    oled_on();

    if (!is_oled_on()) {
        return false;
    }

    if (is_keyboard_master()) {
        render_master(current_keycode, loop_rate);
    } else {
        static bool oled_slave_init_done = false;
        if (!oled_slave_init_done) {
            oled_slave_init_done = true;
            oled_reinit_slave();
        }
        render_slave();
    }

    return false;
}

#include "display/oledWidget/wpm_widget.h"

void render_slave_keymap(uint8_t layer, bool force_redraw) {
    // Top rows: WPM
    render_wpm(0, 0, force_redraw);

    // Bottom rows: Right Encoder mode
    if (force_redraw) {
        oled_set_cursor(0, 12);
        oled_write("Enc  ", false);
        oled_set_cursor(0, 13);
        oled_write("-----", false);
    }

    oled_set_cursor(0, 14);
    uint8_t mode = current_split_state.keymap_data[0];
    switch (mode) {
        case ENC1_MODE_SCROLL:
            oled_write("Scr", false);
            break;
        case ENC1_MODE_DESKTOP:
            oled_write("Dsk", false);
            break;
        case ENC1_MODE_BRIGHTNESS:
            oled_write("Brt", false);
            break;
        default:
            oled_write("???", false);
    }

    oled_set_cursor(0, 15);
    oled_write("     ", false);
}
