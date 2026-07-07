// Copyright 2023-2024 Juraj Fiala (@jurf)
// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "features/encoder.h"
#include "keymap.h"
#include "r58iiz.h"
#include "rgb/rgb.h"
#include "state/split_state.h"

enum {
    U_TD_BOOT = 0,
#define MIRYOKU_X(LAYER) U_TD_U_##LAYER,
    MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

#define U_NP KC_NO // key is not present
#define U_NA KC_NO // present but not available for use
#define U_NU KC_NO // available but not used

// Layer name on OLED
const char *layer_string(uint32_t layer) {
    switch (layer) {
#define MIRYOKU_X(LAYER) \
    case U_##LAYER:      \
        return #LAYER;
        MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
        default:
            return "?";
    }
}

bool is_base_layer(const uint8_t layer) {
    return layer >= U_BASE && layer < U_BUTTON;
}


// Tap dance actions
void u_td_fn_boot(tap_dance_state_t *state, void *user_data) {
    if (state->count == 2) {
        reset_keyboard();
    }
}

#define MIRYOKU_X(LAYER)                                                \
    void u_td_fn_U_##LAYER(tap_dance_state_t *state, void *user_data) { \
        if (state->count == 2) {                                        \
            default_layer_set((layer_state_t)1 << U_##LAYER);           \
        }                                                               \
    }
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X

tap_dance_action_t tap_dance_actions[] = {[U_TD_BOOT] = ACTION_TAP_DANCE_FN(u_td_fn_boot),
#define MIRYOKU_X(LAYER) [U_TD_U_##LAYER] = ACTION_TAP_DANCE_FN(u_td_fn_U_##LAYER),
                                          MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

// Keymaps definition
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // clang-format off
    [U_GAME] = LAYOUT(
        KC_GRV,            KC_1,              KC_2,              KC_3,              KC_4,              KC_5,                                                    KC_6,              KC_7,              KC_8,              KC_9,              KC_0,                 DF(U_BASE),
        KC_T,              KC_TAB,            KC_Q,              KC_W,              KC_E,              KC_R,                                                    KC_Y,              KC_U,              KC_I,              KC_O,              KC_P,                 KC_LBRC,
        KC_G,              KC_LCTL,           KC_A,              KC_S,              KC_D,              KC_F,                                                    KC_H,              KC_J,              KC_K,              KC_L,              KC_SCLN,              KC_QUOT,
        KC_B,              KC_LSFT,           KC_Z,              KC_X,              KC_C,              KC_V,              KC_ENC0,           KC_ENC1,           KC_N,              KC_M,              KC_COMM,           KC_DOT,            KC_SLSH,              KC_EQL,
                                              TD(U_TD_U_BASE),   KC_LALT,           LT(U_FUN,KC_ESC),  KC_SPC,            KC_LGUI,           KC_ENT,            LT(U_BASE,KC_BSPC),KC_DEL,            KC_LEFT,           KC_RIGHT
    ),
    [U_BASE] = LAYOUT(
        DF(U_1HAND),       KC_1,              KC_2,              KC_3,              KC_4,              KC_5,                                                    KC_6,              KC_7,              KC_8,              KC_9,              KC_0,                 XXXXXXX,
        KC_TAB,            KC_Q,              KC_W,              KC_F,              KC_P,              KC_B,                                                    KC_J,              KC_L,              KC_U,              KC_Y,              KC_SCLN,              KC_MINS,
        CTL_T(KC_MINS),    GUI_T(KC_A),       ALT_T(KC_R),       CTL_T(KC_S),       SFT_T(KC_T),       LT(U_GAME,KC_G),                                         KC_M,              SFT_T(KC_N),       CTL_T(KC_E),       ALT_T(KC_I),       GUI_T(KC_O),          CTL_T(KC_QUOT),
        KC_LSFT,           LT(U_BUTTON,KC_Z), ALGR_T(KC_X),      LT(U_GAME,KC_C),   LT(U_GAME,KC_D),   LT(U_GAME,KC_V),   KC_ENC0,           KC_ENC1,           KC_K,              KC_H,              KC_COMM,           ALGR_T(KC_DOT),    LT(U_BUTTON,KC_SLSH), KC_RSFT,
                                              GUI_T(KC_RIGHT),   ALT_T(KC_RIGHT),   LT(U_MEDIA,KC_ESC),LT(U_NAV,KC_SPC),  LT(U_MOUSE,KC_TAB),LT(U_SYM,KC_ENT),  LT(U_NUM,KC_BSPC), LT(U_FUN,KC_DEL),  KC_LEFT,           KC_RIGHT
    ),
    [U_EXTRA] = LAYOUT(
        DF(U_1HAND),       KC_1,              KC_2,              KC_3,              KC_4,              KC_5,                                                    KC_6,              KC_7,              KC_8,              KC_9,              KC_0,                 XXXXXXX,
        KC_TAB,            KC_Q,              KC_W,              KC_E,              KC_R,              KC_T,                                                    KC_Y,              KC_U,              KC_I,              KC_O,              KC_P,                 KC_MINS,
        CTL_T(KC_MINS),    GUI_T(KC_A),       ALT_T(KC_S),       CTL_T(KC_D),       SFT_T(KC_F),       KC_G,                                                    KC_H,              SFT_T(KC_J),       CTL_T(KC_K),       ALT_T(KC_L),       GUI_T(KC_SCLN),       CTL_T(KC_QUOT),
        KC_LSFT,           LT(U_BUTTON,KC_Z), ALGR_T(KC_X),      KC_C,              KC_V,              KC_B,              KC_ENC0,           KC_ENC1,           KC_N,              KC_M,              KC_COMM,           ALGR_T(KC_DOT),    LT(U_BUTTON,KC_SLSH), KC_RSFT,
                                              GUI_T(KC_RIGHT),   ALT_T(KC_RIGHT),   LT(U_MEDIA,KC_ESC),LT(U_NAV,KC_SPC),  LT(U_MOUSE,KC_TAB),LT(U_SYM,KC_ENT),  LT(U_NUM,KC_BSPC), LT(U_FUN,KC_DEL),  KC_LEFT,           KC_RIGHT
    ),
    [U_TAP] = LAYOUT(
        DF(U_1HAND),       KC_1,              KC_2,              KC_3,              KC_4,              KC_5,                                                    KC_6,              KC_7,              KC_8,              KC_9,              KC_0,                 DF(U_BASE),
        KC_TAB,            KC_Q,              KC_W,              KC_F,              KC_P,              KC_B,                                                    KC_J,              KC_L,              KC_U,              KC_Y,              KC_SCLN,              KC_MINS,
        CTL_T(KC_MINS),    KC_A,              KC_R,              KC_S,              KC_T,              KC_G,                                                    KC_M,              KC_N,              KC_E,              KC_I,              KC_O,                 CTL_T(KC_QUOT),
        KC_LSFT,           KC_Z,              KC_X,              KC_C,              KC_D,              KC_V,              KC_ENC0,           KC_ENC1,           KC_K,              KC_H,              KC_COMM,           KC_DOT,            KC_SLSH,              KC_RSFT,
                                              GUI_T(KC_RIGHT),   ALT_T(KC_RIGHT),   LT(U_MEDIA,KC_ESC),LT(U_NAV,KC_SPC),  LT(U_MOUSE,KC_TAB),LT(U_SYM,KC_ENT),  LT(U_NUM,KC_BSPC), LT(U_FUN,KC_DEL),  KC_LEFT,           KC_RIGHT
    ),
    [U_1HAND] = LAYOUT(
        XXXXXXX,           KC_LEFT,           KC_UP,             KC_DOWN,           KC_RIGHT,          XXXXXXX,                                                 KC_6,              KC_7,              KC_8,              KC_9,              KC_0,                 DF(U_BASE),
        KC_DEL,            KC_Q,              KC_W,              KC_E,              KC_R,              KC_T,                                                    KC_Y,              KC_U,              KC_I,              KC_O,              KC_P,                 KC_LBRC,
        CTL_T(KC_BSPC),    GUI_T(KC_A),       ALT_T(KC_S),       CTL_T(KC_D),       SFT_T(KC_F),       LT(U_GAME,KC_G),                                         KC_H,              SFT_T(KC_J),       CTL_T(KC_K),       ALT_T(KC_L),       GUI_T(KC_SCLN),       KC_QUOT,
        KC_LSFT,           LT(U_BUTTON,KC_Z), ALGR_T(KC_X),      LT(U_GAME,KC_C),   LT(U_GAME,KC_V),   LT(U_GAME,KC_B),   KC_ENC0,           KC_ENC1,           KC_N,              KC_M,              KC_COMM,           ALGR_T(KC_DOT),    LT(U_BUTTON,KC_SLSH), KC_RBRC,
                                              GUI_T(KC_RIGHT),   ALT_T(KC_RIGHT),   LT(U_FUN,KC_ESC),  LT(U_NUM,KC_SPC),  LT(U_SYM,KC_TAB),  LT(U_MOUSE,KC_ENT),LT(U_NAV,KC_BSPC), LT(U_MEDIA,KC_DEL),KC_LEFT,           KC_RIGHT
    ),
    [U_BUTTON] = LAYOUT(
        _______,           _______,           _______,           _______,           _______,           _______,                                                 _______,           _______,           _______,           _______,           _______,              _______,
        _______,           U_UND,             U_CUT,             U_CPY,             U_PST,             U_RDO,                                                   U_RDO,             U_PST,             U_CPY,             U_CUT,             U_UND,                _______,
        _______,           KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NU,                                                    U_NU,              KC_LSFT,           KC_LCTL,           KC_LALT,           KC_LGUI,              _______,
        _______,           U_UND,             U_CUT,             U_CPY,             U_PST,             U_RDO,             _______,           _______,           U_RDO,             U_PST,             U_CPY,             U_CUT,             U_UND,                _______,
                                              U_NP,              U_NP,              MS_BTN3,           MS_BTN1,           MS_BTN2,           MS_BTN2,           MS_BTN1,           MS_BTN3,           U_NP,              U_NP
    ),
    [U_NAV] = LAYOUT(
        _______,           _______,           _______,           _______,           _______,           _______,                                                 KC_CAPS,           _______,           _______,           _______,           _______,              _______,
        _______,           TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   TD(U_TD_U_GAME),                                         U_RDO,             U_PST,             U_CPY,             U_CUT,             U_UND,                _______,
        _______,           KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,                                                    CW_TOGG,           KC_LEFT,           KC_DOWN,           KC_UP,             KC_RGHT,              _______,
        _______,           U_NA,              KC_ALGR,           TD(U_TD_U_NUM),    TD(U_TD_U_NAV),    U_NA,              _______,           _______,           KC_INS,            KC_HOME,           KC_PGDN,           KC_PGUP,           KC_END,               _______,
                                              U_NP,              U_NP,              U_NA,              KC_HGHL,           U_NA,              KC_ENT,            KC_BSPC,           KC_DEL,            U_NP,              U_NP
    ),
    [U_MOUSE] = LAYOUT(
        _______,           _______,           _______,           _______,           _______,           _______,                                                 _______,           _______,           _______,           _______,           _______,              _______,
        _______,           TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   TD(U_TD_U_GAME),                                         U_RDO,             U_PST,             U_CPY,             U_CUT,             U_UND,                _______,
        _______,           KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,                                                    KC_WWW_FORWARD,    MS_LEFT,           MS_DOWN,           MS_UP,             MS_RGHT,              _______,
        _______,           U_NA,              KC_ALGR,           TD(U_TD_U_SYM),    TD(U_TD_U_MOUSE),  U_NA,              _______,           _______,           KC_WWW_BACK,       MS_WHLL,           MS_WHLD,           MS_WHLU,           MS_WHLR,              _______,
                                              U_NP,              U_NP,              U_NA,              U_NA,              KC_HGHL,           MS_BTN2,           MS_BTN1,           MS_BTN3,           U_NP,              U_NP
    ),
    [U_MEDIA] = LAYOUT(
        _______,           _______,           _______,           _______,           _______,           _______,                                                 _______,           RM_NEXT,           RM_HUEU,           RM_SATU,           RM_VALU,              RM_SPDU,
        _______,           TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   TD(U_TD_U_GAME),                                         RM_TOGG,           RM_PREV,           RM_HUED,           RM_SATD,           RM_VALD,              RM_SPDD,
        _______,           KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,                                                    U_NU,              KC_MPRV,           KC_VOLD,           KC_VOLU,           KC_MNXT,              _______,
        _______,           U_NA,              KC_ALGR,           TD(U_TD_U_FUN),    TD(U_TD_U_MEDIA),  U_NA,              _______,           _______,           _______,           CK_OLTG,           U_NU,              U_NU,              U_NU,                 _______,
                                              U_NP,              U_NP,              KC_HGHL,           U_NA,              U_NA,              KC_MSTP,           KC_MPLY,           KC_MUTE,           U_NP,              U_NP
    ),
    [U_NUM] = LAYOUT(
        _______,           _______,           _______,           _______,           _______,           _______,                                                 _______,           _______,           _______,           _______,           _______,              _______,
        KC_MINS,           KC_LBRC,           KC_7,              KC_8,              KC_9,              KC_RBRC,                                                 TD(U_TD_U_GAME),   TD(U_TD_U_BASE),   TD(U_TD_U_EXTRA),  TD(U_TD_U_TAP),    TD(U_TD_BOOT),        _______,
        KC_MINS,           KC_SCLN,           KC_4,              KC_5,              KC_6,              KC_EQL,                                                  U_NA,              KC_LSFT,           KC_LCTL,           KC_LALT,           KC_LGUI,              _______,
        KC_NO,             KC_GRV,            KC_1,              KC_2,              KC_3,              KC_BSLS,           _______,           _______,           U_NA,              TD(U_TD_U_NUM),    TD(U_TD_U_NAV),    KC_ALGR,           U_NA,                 _______,
                                              _______,           KC_0,              KC_DOT,            KC_0,              KC_NO,             U_NA,              KC_HGHL,           U_NA,              U_NP,              U_NP
    ),
    [U_SYM] = LAYOUT(
        _______,           _______,           _______,           _______,           _______,           _______,                                                 _______,           _______,           _______,           _______,           _______,              _______,
        KC_NO,             KC_LCBR,           KC_AMPR,           KC_ASTR,           KC_LPRN,           KC_RCBR,                                                 TD(U_TD_U_GAME),   TD(U_TD_U_BASE),   TD(U_TD_U_EXTRA),  TD(U_TD_U_TAP),    TD(U_TD_BOOT),        _______,
        KC_UNDS,           KC_COLN,           KC_DLR,            KC_PERC,           KC_CIRC,           KC_PLUS,                                                 U_NA,              KC_LSFT,           KC_LCTL,           KC_LALT,           KC_LGUI,              _______,
        _______,           KC_TILD,           KC_EXLM,           KC_AT,             KC_HASH,           KC_PIPE,           _______,           _______,           U_NA,              TD(U_TD_U_SYM),    TD(U_TD_U_MOUSE),  KC_ALGR,           U_NA,                 _______,
                                              _______,           KC_RPRN,           KC_LPRN,           KC_RPRN,           KC_NO,             KC_HGHL,           U_NA,              U_NA,              U_NP,              U_NP
    ),
    [U_FUN] = LAYOUT(
        _______,           _______,           _______,           _______,           _______,           _______,                                                 _______,           _______,           _______,           _______,           _______,              _______,
        _______,           KC_F12,            KC_F7,             KC_F8,             KC_F9,             KC_PSCR,                                                 TD(U_TD_U_GAME),   TD(U_TD_U_BASE),   TD(U_TD_U_EXTRA),  TD(U_TD_U_TAP),    TD(U_TD_BOOT),        _______,
        _______,           KC_F11,            KC_F4,             KC_F5,             KC_F6,             KC_SCRL,                                                 U_NA,              KC_LSFT,           KC_LCTL,           KC_LALT,           KC_LGUI,              _______,
        _______,           KC_F10,            KC_F1,             KC_F2,             KC_F3,             KC_PAUS,           _______,           _______,           U_NA,              TD(U_TD_U_FUN),    TD(U_TD_U_MEDIA),  KC_ALGR,           U_NA,                 _______,
                                              KC_APP,            KC_ESC,            KC_ESC,            KC_SPC,            KC_TAB,            U_NA,              U_NA,              KC_HGHL,             U_NP,              U_NP
    ),
    // clang-format on
};

// Tapping Term Per Key
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 275;
        // Colemak-DH home row mods
        case GUI_T(KC_A):
        case GUI_T(KC_O):
        case LT(U_BUTTON, KC_Z):
        case LT(U_BUTTON, KC_SLSH):
        // Qwerty home row mods
        case GUI_T(KC_SCLN):
            return 240;
        default:
            return TAPPING_TERM;
    }
}


// Keyboard post init
void keyboard_post_init_user(void) {
    override_led_flags();
    default_layer_set((layer_state_t)1 << U_BASE);
}


// Process record keymap callback
bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    if (!handle_encoder_keys(keycode, record)) {
        return false;
    }
    if ((keycode == LT(0, KC_C) || keycode == LT(0, KC_G) || keycode == LT(0, KC_V)
         || keycode == LT(0, KC_D) || keycode == LT(0, KC_B))
        && !record->tap.count) {
        if (record->event.pressed) {
            register_code(KC_SPACE);
        } else {
            unregister_code(KC_SPACE);
        }
        return false;
    }
    return true;
}

// Auto Shift settings
bool get_custom_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    return false;
}

void autoshift_press_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    switch (keycode) {
        case KC_DOT:
            if (IS_LAYER_ON(U_NUM)) {
                register_code16((!shifted) ? KC_DOT : KC_LEFT_PAREN);
                break;
            }
            [[fallthrough]];
        default:
            if (shifted) {
                add_weak_mods(MOD_BIT(KC_LSFT));
            }
            register_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
    }
}

void autoshift_release_user(uint16_t keycode, bool shifted, keyrecord_t *record) {
    switch (keycode) {
        case KC_DOT:
            if (IS_LAYER_ON(U_NUM)) {
                unregister_code16((!shifted) ? KC_DOT : KC_LEFT_PAREN);
                break;
            }
            [[fallthrough]];
        default:
            unregister_code16((IS_RETRO(keycode)) ? keycode & 0xFF : keycode);
    }
}

// Caps Word settings
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case KC_A ... KC_Z:
        case KC_MINS:
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_UNDS:
            return true;
        default:
            return false;
    }
}

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [U_GAME]   = {ENCODER_CCW_CW(ENC0_CC, ENC0_CW), ENCODER_CCW_CW(ENC1_CC, ENC1_CW)},
    [U_BASE]   = {ENCODER_CCW_CW(ENC0_CC, ENC0_CW), ENCODER_CCW_CW(ENC1_CC, ENC1_CW)},
    [U_EXTRA]  = {ENCODER_CCW_CW(ENC0_CC, ENC0_CW), ENCODER_CCW_CW(ENC1_CC, ENC1_CW)},
    [U_TAP]    = {ENCODER_CCW_CW(ENC0_CC, ENC0_CW), ENCODER_CCW_CW(ENC1_CC, ENC1_CW)},
    [U_1HAND]  = {ENCODER_CCW_CW(ENC0_CC, ENC0_CW), ENCODER_CCW_CW(ENC1_CC, ENC1_CW)},
    [U_BUTTON] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [U_NAV]    = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [U_MOUSE]  = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [U_MEDIA]  = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [U_NUM]    = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [U_SYM]    = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [U_FUN]    = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
};
#endif


void split_state_housekeeping_keymap(void) {
    if (current_split_state.keymap_data[0] != enc1_mode) {
        current_split_state.keymap_data[0] = enc1_mode;
        split_state_trigger_sync();
    }
}
