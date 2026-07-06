// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "quantum.h"

void set_system_os(os_variant_t detected_os);
void render_system(uint8_t col, uint8_t row, uint16_t loop_rate, bool force_redraw);
