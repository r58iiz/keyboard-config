// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include QMK_KEYBOARD_H

enum custom_keycodes {
    CK_OLTG = SAFE_RANGE, //
    CK_LITG,              //
    UR_SAFE_RANGE         //
};

bool process_record_keymap(uint16_t keycode, keyrecord_t *record);
bool rgb_matrix_indicators_advanced_keymap(uint8_t led_min, uint8_t led_max);
