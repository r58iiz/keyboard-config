// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rgbm_widget.h"
#include "../oled_utils.h"

void render_rgbm_info(uint8_t col, uint8_t row, bool force_redraw) {
    if (force_redraw) {
        oled_set_cursor(col, row);
        oled_write_ln("RGBM", false);
        render_spacer(3);
        oled_set_cursor(col, row + 2);
        oled_write("M:", false);
    }

    static uint8_t last_mode = 0xFF;

    uint8_t        mode      = rgb_matrix_get_mode();
    char           buf[4];

    if (force_redraw || mode != last_mode) {
        snprintf(buf, sizeof(buf), "%3u", mode);
        last_mode = mode;

        oled_set_cursor(col + 2, row + 2);
        oled_write(buf, false);

#ifndef VIAL_ENABLE
        const char *effect_name = rgb_matrix_get_mode_name(mode);
        oled_set_cursor(col, row + 3);
        oled_write(effect_name, false);
#endif
    }
}
