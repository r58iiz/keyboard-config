// Copyright 2025 Dasky (@daskygit)
// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "r58iiz.h"
#include "state/split_state.h"
#include "display/oledWidget/hsv_widget.h"
#include "display/oledWidget/rgbm_widget.h"
#include "display/oledWidget/wpm_widget.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                              KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                              KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    TT(2),
    KC_LSFT, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                              KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    KC_LCTL, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_MUTE,        KC_MPLY,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                      KC_LGUI, KC_LALT, KC_LCTL, MO(1),   KC_SPC,         KC_ENT,   MO(2),   KC_RCTL, KC_RALT, KC_RGUI
  ),

  [1] = LAYOUT(
    KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                             KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                              KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_F12,
    _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                           KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PIPE,
    _______, KC_EQL,  KC_MINS, KC_PLUS, KC_LCBR, KC_RCBR, XXXXXXX,        XXXXXXX,  KC_LBRC, KC_RBRC, KC_SCLN, KC_COLN, KC_BSLS, _______,
                      _______, _______, _______, _______, XXXXXXX,        XXXXXXX,  MO(3),   _______, _______, _______
  ),

  [2] = LAYOUT(
    KC_ESC,  XXXXXXX, MS_BTN1, MS_BTN3, MS_BTN2, XXXXXXX,                           MS_ACL0, MS_ACL1, MS_ACL2, XXXXXXX, XXXXXXX, KC_DEL,
    MS_WHLL, MS_WHLU, XXXXXXX, MS_UP,   XXXXXXX, XXXXXXX,                           KC_PGUP, XXXXXXX, KC_UP,   XXXXXXX, XXXXXXX, TT(2),
    MS_WHLR, MS_WHLD, MS_LEFT, MS_DOWN, MS_RGHT, KC_CAPS,                           KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_DEL,  XXXXXXX,
    MS_BTN2, C(KC_Z), C(KC_X), C(KC_C), C(KC_V), XXXXXXX, XXXXXXX,        XXXXXXX,  XXXXXXX, KC_INS,  KC_PSCR, KC_MENU, XXXXXXX, KC_PAUS,
                      _______, _______, _______, MO(3),   MS_BTN1,        MS_BTN2,  XXXXXXX, _______, _______, _______
  ),

  [3] = LAYOUT(
    CK_OLTG,  CK_LITG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    RM_NEXT,  RM_HUEU, RM_SATU, RM_VALU, XXXXXXX, XXXXXXX,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    RM_TOGG,  RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX, XXXXXXX,        XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,        XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
  )
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
  [0] = {  ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_MPRV, KC_MNXT)                          },
  [1] = {  ENCODER_CCW_CW(KC_TRNS, KC_TRNS),  ENCODER_CCW_CW(KC_TRNS, KC_TRNS)                          },
  [2] = {  ENCODER_CCW_CW(KC_BRID, KC_BRIU),  ENCODER_CCW_CW(LCTL(LGUI(KC_LEFT)), LCTL(LGUI(KC_RGHT)))  },
  [3] = {  ENCODER_CCW_CW(KC_TRNS, KC_TRNS),  ENCODER_CCW_CW(KC_TRNS, KC_TRNS)                          }
};
#endif
// clang-format on

#ifndef VIAL_ENABLE
const uint16_t PROGMEM c1[]         = {KC_GRV, KC_TAB, COMBO_END};
combo_t                key_combos[] = {
    COMBO(c1, KC_ESC),
};
#endif

#ifdef RGB_MATRIX_ENABLE
static bool rgb_enabled = false;
static uint8_t rgb_mode = 0;

void split_state_oled_change_keymap(bool enabled) {
    if (!enabled) {
        rgb_enabled = rgb_matrix_is_enabled();
        rgb_mode    = rgb_matrix_get_mode();
        if (!rgb_enabled) {
            rgb_matrix_enable_noeeprom();
            rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_my_blank_effect);
        }
    } else {
        if (!rgb_enabled) {
            rgb_matrix_mode_noeeprom(rgb_mode);
            rgb_matrix_disable_noeeprom();
        }
    }
}

bool rgb_matrix_indicators_advanced_keymap(uint8_t led_min, uint8_t led_max) {
    if (!is_layer_rgb_indicator_enabled())
        return false;

    static uint8_t  last_layer  = 0;
    static uint32_t layer_timer = 0;
    static bool     flashing    = false;

    uint8_t         layer       = get_highest_layer(layer_state | default_layer_state);

    if (layer != last_layer) {
        uint8_t prev_layer = last_layer;
        last_layer         = layer;

        if (!is_oled_active()) {
            layer_timer = timer_read();
            flashing    = (layer != 2 && layer > prev_layer);
        }
    }

    if (!is_oled_active() && layer == 2) {
        rgb_matrix_set_color(33, RGB_GREEN);
        return false;
    }

    if (!is_oled_active() && flashing && timer_elapsed(layer_timer) < 100) {
        rgb_t rgb = {0, 0, 0};
        switch (layer) {
            case 0:
                rgb = (rgb_t){RGB_CORAL};
                break;
            case 1:
                rgb = (rgb_t){RGB_RED};
                break;
            case 3:
                rgb = (rgb_t){RGB_BLUE};
                break;
        }
        rgb_matrix_set_color(33, rgb.r, rgb.g, rgb.b);
    } else {
        flashing = false;
    }

    return false;
}
#endif

#ifdef OLED_ENABLE
void render_slave_keymap(uint8_t layer, bool force_redraw) {
    switch (layer) {
        case 3:
            render_hsv_info(0, 0, force_redraw);
            render_rgbm_info(0, 6, force_redraw);
            break;
        default:
            render_wpm(0, 0, force_redraw);
            break;
    }
}
#endif
