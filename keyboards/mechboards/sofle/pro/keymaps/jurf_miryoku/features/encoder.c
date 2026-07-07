// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "../keymap.h"
#include "encoder.h"

uint8_t enc0_mode = ENC0_MODE_UNDOREDO;
uint8_t enc1_mode = ENC1_MODE_SCROLL;

bool handle_encoder_keys(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case KC_ENC0:
            enc0_mode = (enc0_mode + 1) % ENC0_MODE_COUNT;
            break;

        case KC_ENC1:
            enc1_mode = (enc1_mode + 1) % ENC1_MODE_COUNT;
            break;

        case ENC0_CW:
            switch (enc0_mode) {
                case ENC0_MODE_UNDOREDO:
                    tap_code16(U_UND);
                    break;
                case ENC0_MODE_VOLUME:
                    tap_code(KC_VOLU);
                    break;
            }
            break;

        case ENC0_CC:
            switch (enc0_mode) {
                case ENC0_MODE_UNDOREDO:
                    tap_code16(U_RDO);
                    break;
                case ENC0_MODE_VOLUME:
                    tap_code(KC_VOLD);
                    break;
            }
            break;

        case ENC1_CW:
            switch (enc1_mode) {
                case ENC1_MODE_SCROLL:
                    tap_code(KC_PGUP);
                    break;
                case ENC1_MODE_DESKTOP:
                    tap_code16(C(G(KC_RIGHT)));
                    break;
                case ENC1_MODE_BRIGHTNESS:
                    tap_code(KC_BRIU);
                    break;
            }
            break;

        case ENC1_CC:
            switch (enc1_mode) {
                case ENC1_MODE_SCROLL:
                    tap_code(KC_PGDN);
                    break;
                case ENC1_MODE_DESKTOP:
                    tap_code16(C(G(KC_LEFT)));
                    break;
                case ENC1_MODE_BRIGHTNESS:
                    tap_code(KC_BRID);
                    break;
            }
            break;

        default:
            return true;
    }

    return false;
}
