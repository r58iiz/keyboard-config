// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef RGB_MATRIX_MODE_NAME_ENABLE
void render_rgbm_info(uint8_t col, uint8_t row, bool force_redraw);
#else
static inline void render_rgbm_info(uint8_t col, uint8_t row, bool force_redraw) {
}
#endif

