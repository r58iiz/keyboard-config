// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_slave.h"
#include "oledWidget/hsv_widget.h"
#include "oledWidget/rgbm_widget.h"
#include "oledWidget/wpm_widget.h"

__attribute__((weak)) void render_slave_keymap(uint8_t layer, bool force_redraw) {
    render_wpm(0, 0, force_redraw);
}

__attribute__((weak)) void render_slave(void) {
    uint8_t        layer        = get_highest_layer(layer_state | default_layer_state);
    static uint8_t last_layer   = 0xFF;
    bool           force_redraw = false;

    if (layer != last_layer) {
        oled_clear();
        last_layer   = layer;
        force_redraw = true;
    }

    render_slave_keymap(layer, force_redraw);
}
