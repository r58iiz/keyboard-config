// Copyright 2025 Dasky (@daskygit)
// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "state/split_state.h"
#include "display/oled_master.h"
#include "display/oled_slave.h"
#include "display/oledWidget/system_widget.h"

extern uint16_t loop_rate;
static uint16_t current_keycode = 0xFF;

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    }
    return rotation;
}

bool process_detected_host_os_kb(os_variant_t detected_os) {
    if (!process_detected_host_os_user(detected_os)) {
        return false;
    }
    set_system_os(detected_os);
    return true;
}

void keyboard_post_init_kb(void) {
    split_state_init();
    keyboard_post_init_user();
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    state = layer_state_set_user(state);
    return state;
}

layer_state_t default_layer_state_set_kb(layer_state_t state) {
    state = default_layer_state_set_user(state);
    return state;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    current_keycode = keycode;
    return process_record_user(keycode, record);
}

uint16_t loop_rate = 0;
void     housekeeping_task_kb(void) {
    split_state_housekeeping();
    if (is_keyboard_master()) {
        static uint32_t     loop_count = 0;
        static fast_timer_t loop_time  = 0;
        loop_count++;
        if (timer_elapsed_fast(loop_time) > 1000) {
            loop_time  = timer_read_fast();
            loop_rate  = loop_count > UINT16_MAX ? UINT16_MAX : loop_count;
            loop_count = 0;
        }
    }
}

void oled_reinit_slave(void) {
    oled_init(OLED_ROTATION_270);
    oled_clear();
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }

    if (!is_oled_active()) {
        oled_off();
        return false;
    }
    oled_on();

    if (!is_oled_on()) {
        return false;
    }

    if (is_keyboard_master()) {
        render_master(current_keycode, loop_rate);
    } else {
        static bool oled_slave_init_done = false;
        if (!oled_slave_init_done) {
            oled_slave_init_done = true;
            oled_reinit_slave();
        }
        render_slave();
    }

    return false;
}
