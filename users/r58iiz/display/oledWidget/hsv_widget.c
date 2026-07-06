// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdio.h>

#include "hsv_widget.h"
#include "../oled_utils.h"

void render_hsv_info(uint8_t col, uint8_t row, bool force_redraw) {
    if (force_redraw) {
        oled_set_cursor(col, row);
        oled_write_ln("HSV", false);
        render_spacer(3);
        oled_set_cursor(col, row + 2);
        oled_write("H:", false);
        oled_set_cursor(col, row + 3);
        oled_write("S:", false);
        oled_set_cursor(col, row + 4);
        oled_write("V:", false);
    }

    static uint8_t last_hue = 0xFF;
    static uint8_t last_sat = 0xFF;
    static uint8_t last_val = 0xFF;

    uint8_t        hue      = rgb_matrix_get_hue();
    uint8_t        sat      = rgb_matrix_get_sat();
    uint8_t        val      = rgb_matrix_get_val();

    char           buf[4];

    if (force_redraw || hue != last_hue) {
        snprintf(buf, sizeof(buf), "%3u", hue);
        last_hue = hue;

        oled_set_cursor(col + 2, row + 2);
        oled_write(buf, false);
    }

    if (force_redraw || sat != last_sat) {
        snprintf(buf, sizeof(buf), "%3u", sat);
        last_sat = sat;

        oled_set_cursor(col + 2, row + 3);
        oled_write(buf, false);
    }

    if (force_redraw || val != last_val) {
        snprintf(buf, sizeof(buf), "%3u", val);
        last_val = val;

        oled_set_cursor(col + 2, row + 4);
        oled_write(buf, false);
    }
}
