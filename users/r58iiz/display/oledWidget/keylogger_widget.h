// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdint.h>
#include <stdbool.h>

const char *keycode_string(uint16_t keycode);
void        render_keylogger(uint8_t col, uint8_t row, uint16_t keycode, bool force_redraw);
