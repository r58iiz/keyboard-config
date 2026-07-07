// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "quantum.h"

// Left encoder mode indices
enum encoder0_modes {
    ENC0_MODE_UNDOREDO = 0,
    ENC0_MODE_VOLUME,
    ENC0_MODE_COUNT
};

// Right encoder mode indices
enum encoder1_modes {
    ENC1_MODE_SCROLL = 0,
    ENC1_MODE_DESKTOP,
    ENC1_MODE_BRIGHTNESS,
    ENC1_MODE_COUNT
};

extern uint8_t enc0_mode, enc1_mode;

bool handle_encoder_keys(uint16_t keycode, keyrecord_t *record);
