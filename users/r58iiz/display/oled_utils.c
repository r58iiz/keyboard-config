// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_utils.h"

const char *depad_str(const char *depad_str, char depad_char) {
    while (*depad_str == depad_char)
        ++depad_str;
    return depad_str;
}

void render_spacer(uint8_t char_length) {
    static const char PROGMEM spacer_char[] = {8, 8, 8, 8, 8, 8, 8};
    if (char_length > 5) {
        char_length = 5;
    }
    for (uint8_t i = 0; i < char_length; i++) {
        oled_write_raw_P(spacer_char, sizeof(spacer_char));
        oled_advance_char();
    }
}
