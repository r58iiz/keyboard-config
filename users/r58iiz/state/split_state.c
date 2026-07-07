// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <string.h>
#include "transactions.h"
#include "split_state.h"

// State

static bool     is_oled_locked_off  = false;
static uint32_t oled_timer          = 0;

split_state_t   current_split_state = {
      .oled_enabled                = true,
      .oled_asleep                 = false,
      .layer_rgb_indicator_enabled = false,
      .keymap_data                 = {0},
};

// Functions

void oled_timer_reset(void) {
    oled_timer = timer_read32();
}

void toggle_oled(void) {
    is_oled_locked_off = !is_oled_locked_off;
}

void toggle_layer_rgb_indicator(void) {
    current_split_state.layer_rgb_indicator_enabled =
        !current_split_state.layer_rgb_indicator_enabled;
}

bool is_oled_active(void) {
    return current_split_state.oled_enabled && !current_split_state.oled_asleep;
}

bool is_layer_rgb_indicator_enabled(void) {
    return current_split_state.layer_rgb_indicator_enabled;
}

void split_state_sync_handler(uint8_t     in_buflen,
                              const void *in_data,
                              uint8_t     out_buflen,
                              void       *out_data) {
    memcpy(&current_split_state, in_data, in_buflen);
}

static bool split_needs_sync = false;

void split_state_trigger_sync(void) {
    split_needs_sync = true;
}

__attribute__((weak)) void split_state_housekeeping_keymap(void) {}
__attribute__((weak)) void split_state_oled_change_keymap(bool enabled) {}

void split_state_init(void) {
    transaction_register_rpc(RPC_SPLIT_SYNC, split_state_sync_handler);
}

void split_state_housekeeping(void) {
    if (!is_keyboard_master())
        return;

    static bool     last_enabled = true;
    static bool     last_asleep  = false;
    static uint32_t last_sync    = 0;

    bool            new_enabled  = !is_oled_locked_off;
    bool            new_asleep = new_enabled && (timer_elapsed32(oled_timer) > CUSTOM_OLED_TIMEOUT);

    split_needs_sync = false;

    if (new_enabled != last_enabled) {
        split_state_oled_change_keymap(new_enabled);
        split_needs_sync = true;
    }

    // Keymap-specific housekeeping callback
    split_state_housekeeping_keymap();

    if (new_asleep != last_asleep) {
        split_needs_sync = true;
    }

    if (timer_elapsed32(last_sync) > 250) {
        split_needs_sync = true;
    }

    current_split_state.oled_enabled = new_enabled;
    current_split_state.oled_asleep  = new_asleep;
    last_enabled                     = new_enabled;
    last_asleep                      = new_asleep;

    if (split_needs_sync) {
        if (transaction_rpc_send(RPC_SPLIT_SYNC,
                                 sizeof(current_split_state),
                                 &current_split_state)) {
            last_sync = timer_read32();
        }
    }
}
