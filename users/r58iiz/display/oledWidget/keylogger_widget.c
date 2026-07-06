// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "keylogger_widget.h"
#include "../oled_utils.h"

char        basic_codes_to_name[57] = {' ', ' ',  ' ', ' ', 'a',  'b', 'c', 'd', 'e', 'f', 'g', 'h',
                                       'i', 'j',  'k', 'l', 'm',  'n', 'o', 'p', 'q', 'r', 's', 't',
                                       'u', 'v',  'w', 'x', 'y',  'z', '1', '2', '3', '4', '5', '6',
                                       '7', '8',  '9', '0', 'R',  'E', 'B', 'T', '_', '-', '=', '[',
                                       ']', '\\', '#', ';', '\'', '`', ',', '.', '/'};

__attribute__((weak)) const char *keycode_string(uint16_t keycode) {
    char       *keycode_str;
    static char key;
    switch (keycode) {
        case 0 ... 56:
            key = pgm_read_byte(&basic_codes_to_name[keycode]);
            return &key;
        case KC_CAPS:
            keycode_str = "Caps\0";
            break;
        case KC_SCRL:
            keycode_str = "Scrl\0";
            break;
        case KC_PAUS:
            keycode_str = "Pause\0";
            break;
        case KC_DEL:
            keycode_str = "Del\0";
            break;
        case KC_NUM:
            keycode_str = "Num\0";
            break;
        case KC_MUTE:
            keycode_str = "Mute\0";
            break;
        case KC_VOLU:
            keycode_str = "VolUp\0";
            break;
        case KC_VOLD:
            keycode_str = "VolD\0";
            break;
        case KC_MNXT:
            keycode_str = "Next\0";
            break;
        case KC_MPRV:
            keycode_str = "Prev\0";
            break;
        case KC_MSTP:
            keycode_str = "Stop\0";
            break;
        case KC_MPLY:
            keycode_str = "Play\0";
            break;
        case QK_MODS ... QK_MODS_MAX:
            keycode_str = "MOD()\0";
            break;
        case QK_MOD_TAP ... QK_MOD_TAP_MAX:
            keycode_str = "MT()\0";
            break;
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
            keycode_str = "LT()\0";
            break;
        case QK_LAYER_MOD ... QK_LAYER_MOD_MAX:
            keycode_str = "LM()\0";
            break;
        case QK_TO ... QK_TO_MAX:
            keycode_str = "TO()\0";
            break;
        case QK_MOMENTARY ... QK_MOMENTARY_MAX:
            keycode_str = "MO()\0";
            break;
        case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
            keycode_str = "DF()\0";
            break;
        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
            keycode_str = "TG()\0";
            break;
        case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_MOD_MAX:
            keycode_str = "1SHOT\0";
            break;
        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
            keycode_str = "TT()\0";
            break;
        case QK_PERSISTENT_DEF_LAYER ... QK_PERSISTENT_DEF_LAYER_MAX:
            keycode_str = "PDF()\0";
            break;
        case QK_SWAP_HANDS ... QK_SWAP_HANDS_MAX:
            keycode_str = "SWAP\0";
            break;
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            keycode_str = "TD()\0";
            break;
        case QK_MAGIC ... QK_MAGIC_MAX:
            keycode_str = "Magic\0";
            break;
        case QK_MIDI ... QK_MIDI_MAX:
            keycode_str = "Midi\0";
            break;
        case QK_SEQUENCER ... QK_SEQUENCER_MAX:
            keycode_str = "Seq\0";
            break;
        case QK_JOYSTICK ... QK_JOYSTICK_MAX:
            keycode_str = "Joy\0";
            break;
        case QK_PROGRAMMABLE_BUTTON ... QK_PROGRAMMABLE_BUTTON_MAX:
            keycode_str = "Prog\0";
            break;
        case QK_AUDIO ... QK_AUDIO_MAX:
            keycode_str = "Audio\0";
            break;
        case QK_STENO ... QK_STENO_MAX:
            keycode_str = "Steno\0";
            break;
        case QK_MACRO ... QK_MACRO_MAX:
            keycode_str = "Macro\0";
            break;
        case QK_CONNECTION ... QK_CONNECTION_MAX:
            keycode_str = "Conn\0";
            break;
        case QK_LIGHTING ... QK_LIGHTING_MAX:
            keycode_str = "Light\0";
            break;
        case QK_QUANTUM ... QK_QUANTUM_MAX:
            keycode_str = "Quant\0";
            break;
        case QK_KB ... QK_KB_MAX:
            keycode_str = "KB\0";
            break;
        case QK_USER ... QK_USER_MAX:
            keycode_str = "USER\0";
            break;
        case QK_UNICODEMAP ... QK_UNICODEMAP_PAIR_MAX:
            keycode_str = "Uni\0";
            break;
        default:
            keycode_str = "Undef\0";
            break;
    }

    return keycode_str;
}

void render_keylogger(uint8_t col, uint8_t row, uint16_t keycode, bool force_redraw) {
    if (force_redraw) {
        oled_set_cursor(col, row);
        oled_write_ln("Key", false);
        render_spacer(3);
    }

    static uint16_t last_keycode = 0xFFFF;
    if (force_redraw || last_keycode != keycode) {
        last_keycode = keycode;
        oled_set_cursor(col, row + 2);
        if (keycode < ARRAY_SIZE(basic_codes_to_name)) {
            oled_write_char(basic_codes_to_name[keycode], false);
            oled_advance_page(true);
        } else {
            oled_write_ln(keycode_string(keycode), false);
        }
    }
}
