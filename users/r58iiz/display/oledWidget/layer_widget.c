// Copyright 2025 Dasky (@daskygit)
// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "layer_widget.h"
#include "../oled_utils.h"

__attribute__((weak)) const char *layer_string(uint32_t layer) {
    char *layer_str;
    switch (layer) {
        case 0:
            layer_str = "Zero\0";
            break;
        case 1:
            layer_str = "One\0";
            break;
        case 2:
            layer_str = "Two\0";
            break;
        case 3:
            layer_str = "Three\0";
            break;
        case 4:
            layer_str = "Four\0";
            break;
        case 5:
            layer_str = "Five\0";
            break;
        case 6:
            layer_str = "Six\0";
            break;
        case 7:
            layer_str = "Seven\0";
            break;
        default:
            return get_u16_str(layer, ' ');
    }

    return layer_str;
}

void render_layer(uint8_t       col,
                  uint8_t       row,
                  layer_state_t momentary_state,
                  layer_state_t default_state,
                  bool          force_redraw) {
    if (force_redraw) {
        oled_set_cursor(col, row);
        oled_write("Layer", false);
        render_spacer(5);
    }

    static uint8_t last_layer    = 0xFF;
    uint8_t        current_layer = get_highest_layer(momentary_state | default_state);
    if (force_redraw || last_layer != current_layer) {
        last_layer = current_layer;
        oled_set_cursor(col, row + 2);
        oled_write_ln(layer_string(current_layer), false);
    }
}
