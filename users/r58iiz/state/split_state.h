// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "quantum.h"

typedef struct {
        bool    oled_enabled;
        bool    oled_asleep;
        bool    layer_rgb_indicator_enabled;
        uint8_t keymap_data[4];
} split_state_t;

extern split_state_t current_split_state;

void                 toggle_oled(void);
bool                 is_oled_active(void);
void                 oled_timer_reset(void);

bool                 is_layer_rgb_indicator_enabled(void);
void                 toggle_layer_rgb_indicator(void);

void                 split_state_init(void);
void                 split_state_housekeeping(void);
void                 split_state_oled_change_keymap(bool enabled);
void                 split_state_housekeeping_keymap(void);
void                 split_state_trigger_sync(void);
