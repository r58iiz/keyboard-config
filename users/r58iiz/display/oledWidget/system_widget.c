// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "system_widget.h"
#include "../oled_utils.h"

static os_variant_t current_os = OS_UNSURE;

void                set_system_os(os_variant_t detected_os) {
    current_os = detected_os;
}

void render_system(uint8_t col, uint8_t row, uint16_t loop_rate, bool force_redraw) {
    if (force_redraw) {
        oled_set_cursor(col, row);
        oled_write_ln("OS", false);
        render_spacer(2);

        oled_set_cursor(col, row + 2);
        switch (current_os) {
            case OS_MACOS:
                oled_write_ln("MacOS", false);
                break;
            case OS_IOS:
                oled_write_ln("Apple", false);
                break;
            case OS_WINDOWS:
                oled_write_ln("Win", false);
                break;
            case OS_LINUX:
                oled_write_ln("Linux", false);
                break;
            case OS_UNSURE:
                oled_write_ln("Unkno", false);
                break;
        }

        oled_set_cursor(col, row + 4);
        oled_write_ln("Rate", false);
        render_spacer(4);
    }

    static uint16_t last_loop_rate = 0xFFFF;
    if (force_redraw || last_loop_rate != loop_rate) {
        last_loop_rate = loop_rate;
        oled_set_cursor(col, row + 6);
        oled_write(depad_str(get_u16_str(loop_rate, ' '), ' '), false);
    }
}
