// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "quantum.h"

const char *layer_string(uint32_t layer);
void        render_layer(uint8_t       col,
                         uint8_t       row,
                         layer_state_t momentary_state,
                         layer_state_t default_state,
                         bool          force_redraw);
