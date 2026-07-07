// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "r58iiz.h"
#include "state/split_state.h"
#include "timer.h"

#ifdef RAW_ENABLE
#include "raw_hid.h"
#endif


// Raw HID stuff

#ifdef RAW_ENABLE
#if defined(VIAL_ENABLE) || defined(VIA_ENABLE)
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
    return false;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    return rgb_matrix_indicators_advanced_keymap(led_min, led_max);
}
