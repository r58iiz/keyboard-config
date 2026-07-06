// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "wpm_widget.h"
#include "../oled_utils.h"

void render_wpm(uint8_t col, uint8_t row, bool force_redraw) {
    if (force_redraw) {
        oled_set_cursor(col, row);
        oled_write_ln("WPM", false);
        render_spacer(3);
    }

    static uint16_t last_wpm    = 0xFFFF;
    uint16_t        current_wpm = get_current_wpm();
    if (force_redraw || last_wpm != current_wpm) {
        last_wpm = current_wpm;
        oled_set_cursor(col, row + 2);
        oled_write_ln(depad_str(get_u16_str(current_wpm, ' '), ' '), false);
    }
}
