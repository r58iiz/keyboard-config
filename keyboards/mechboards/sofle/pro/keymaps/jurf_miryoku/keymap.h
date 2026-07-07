// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdbool.h>
#include "quantum.h"
#include "r58iiz.h"

// Define custom keycodes
enum custom_keycodes_keymap {
    KC_HGHL = UR_SAFE_RANGE,
    KC_ENC0,
    ENC0_CW,
    ENC0_CC,
    KC_ENC1,
    ENC1_CC,
    ENC1_CW
};

// Define layers
#define MIRYOKU_LAYER_LIST  \
    MIRYOKU_X(GAME)   \
    MIRYOKU_X(BASE)   \
    MIRYOKU_X(EXTRA)  \
    MIRYOKU_X(TAP)    \
    MIRYOKU_X(1HAND)  \
    MIRYOKU_X(BUTTON) \
    MIRYOKU_X(NAV)    \
    MIRYOKU_X(MOUSE)  \
    MIRYOKU_X(MEDIA)  \
    MIRYOKU_X(NUM)    \
    MIRYOKU_X(SYM)    \
    MIRYOKU_X(FUN)

enum miryoku_layers {
#define MIRYOKU_X(LAYER) U_##LAYER,
    MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

#define U_RDO C(S(KC_Z))
#define U_PST C(KC_V)
#define U_CPY C(KC_C)
#define U_CUT C(KC_X)
#define U_UND C(KC_Z)

bool is_base_layer(const uint8_t layer);
const char *layer_string(uint32_t layer);
