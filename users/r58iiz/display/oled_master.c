// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_master.h"
#include "oledWidget/keylogger_widget.h"
#include "oledWidget/layer_widget.h"
#include "oledWidget/system_widget.h"

__attribute__((weak)) void render_master(uint16_t current_keycode, uint16_t loop_rate) {
    uint8_t        layer        = get_highest_layer(layer_state | default_layer_state);
    static uint8_t last_layer   = 0xFF;
    bool           force_redraw = false;

    if (layer != last_layer) {
        oled_clear();
        last_layer   = layer;
        force_redraw = true;
    }

    switch (layer) {
        default:
            render_layer(0, 0, layer_state, default_layer_state, force_redraw);
            render_keylogger(0, 4, current_keycode, force_redraw);
            render_system(0, 8, loop_rate, force_redraw);
            break;
    }
}
