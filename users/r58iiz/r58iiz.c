// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "r58iiz.h"
#include "state/split_state.h"
#include "timer.h"


// Raw HID stuff

#ifdef RAW_ENABLE
#ifdef VIAL_ENABLE
void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
#else
void raw_hid_receive(uint8_t *data, uint8_t length) {
#endif
    uint8_t response[length];
    memset(response, 0, length);

    if (data[0] == 0xFF) {
        if (data[1] == 0x01) { // Current Active Layer
            response[0] = 0xFF;
            response[1] = 0x01;
            response[2] = get_highest_layer(layer_state);

#ifdef VIAL_ENABLE
            host_raw_hid_send(response, length);
#else
            raw_hid_send(response, length);
#endif
            return;
        }
    }
}
#endif



// Keypress

__attribute__((weak)) bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        oled_timer_reset();
    }

    if (keycode == CK_OLTG && record->event.pressed) {
        toggle_oled();
        return false;
    } else if (keycode == CK_LITG && record->event.pressed) {
        toggle_layer_rgb_indicator();
        return false;
    }
    return process_record_keymap(keycode, record);
}


// RGB Matrix

__attribute__((weak)) bool rgb_matrix_indicators_advanced_keymap(uint8_t led_min, uint8_t led_max) {
    if (!current_split_state.layer_rgb_indicator_enabled)
        return false;

    static uint8_t  last_layer  = 0;
    static uint32_t layer_timer = 0;
    static bool     flashing    = false;

    uint8_t         layer       = get_highest_layer(layer_state | default_layer_state);

    if (layer != last_layer) {
        uint8_t prev_layer = last_layer;
        last_layer         = layer;

        if (!current_split_state.oled_enabled) {
            layer_timer = timer_read();
            flashing    = (layer != 2 && layer > prev_layer);
        }
    }

    if (!current_split_state.oled_enabled && layer == 2) {
        rgb_matrix_set_color(33, RGB_GREEN);
        return false;
    }

    if (!current_split_state.oled_enabled && flashing && timer_elapsed(layer_timer) < 100) {
        rgb_t rgb = {0, 0, 0};
        switch (layer) {
            case 0:
                rgb = (rgb_t){RGB_CORAL};
                break;
            case 1:
                rgb = (rgb_t){RGB_RED};
                break;
            case 3:
                rgb = (rgb_t){RGB_BLUE};
                break;
        }
        rgb_matrix_set_color(33, rgb.r, rgb.g, rgb.b);
    } else {
        flashing = false;
    }

    return false;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    return rgb_matrix_indicators_advanced_keymap(led_min, led_max);
}
