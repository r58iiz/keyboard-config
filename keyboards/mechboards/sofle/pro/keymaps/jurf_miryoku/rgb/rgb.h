// Copyright 2023-2024 Juraj Fiala (@jurf)
// Copyright 2026 r58iiz (@r58iiz)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <stdint.h>
#include "quantum.h"


#define LOWINTENSITY
#ifdef ORIGINAL
// Gamma-corrected Solarized colors
    #define RGB_SOLARIZED_BASE03  0, 6, 9
    #define RGB_SOLARIZED_BASE02  1, 9, 14
    #define RGB_SOLARIZED_BASE01  25, 40, 45
    #define RGB_SOLARIZED_BASE00  33, 51, 58
    #define RGB_SOLARIZED_BASE0   58, 76, 78
    #define RGB_SOLARIZED_BASE1   74, 91, 91
    #define RGB_SOLARIZED_BASE2   218, 206, 170
    #define RGB_SOLARIZED_BASE3   250, 235, 196
    #define RGB_SOLARIZED_YELLOW  118, 64, 0
    #define RGB_SOLARIZED_ORANGE  152, 18, 2
    #define RGB_SOLARIZED_RED     183, 8, 7
    #define RGB_SOLARIZED_MAGENTA 166, 9, 57
    #define RGB_SOLARIZED_VIOLET  38, 42, 141
    #define RGB_SOLARIZED_BLUE    5, 66, 164
    #define RGB_SOLARIZED_CYAN    6, 91, 80
    #define RGB_SOLARIZED_GREEN   60, 81, 0
#elifdef LOWINTENSITY
    #define DIM_RGB(r, g, b, pct) ((r) * (pct) / 100), ((g) * (pct) / 100), ((b) * (pct) / 100)
    #define DIM(r, g, b)          DIM_RGB(r, g, b, 90)

    // Low-intensity colors
    #define RGB_SOLARIZED_BASE03  DIM(0, 4, 6)
    #define RGB_SOLARIZED_BASE02  DIM(1, 6, 9)
    #define RGB_SOLARIZED_BASE01  DIM(14, 22, 25)
    #define RGB_SOLARIZED_BASE00  DIM(18, 28, 32)
    #define RGB_SOLARIZED_BASE0   DIM(28, 38, 40)
    #define RGB_SOLARIZED_BASE1   DIM(36, 45, 45)
    #define RGB_SOLARIZED_BASE2   DIM(70, 65, 52)
    #define RGB_SOLARIZED_BASE3   DIM(85, 78, 60)
    #define RGB_SOLARIZED_YELLOW  DIM(62, 40, 8)
    #define RGB_SOLARIZED_ORANGE  DIM(70, 24, 6)
    #define RGB_SOLARIZED_RED     DIM(78, 14, 12)
    #define RGB_SOLARIZED_MAGENTA DIM(68, 12, 32)
    #define RGB_SOLARIZED_VIOLET  DIM(24, 22, 58)
    #define RGB_SOLARIZED_BLUE    DIM(10, 32, 68)
    #define RGB_SOLARIZED_CYAN    DIM(8, 46, 40)
    #define RGB_SOLARIZED_GREEN   DIM(30, 40, 6)
#elifdef DESATURATED_ORIGINAL
    // Claude: Desaturate a color by `pct` percent, keeping HSV Value (max channel) fixed.
    #define _MAX2(a, b)          ((a) > (b) ? (a) : (b))
    #define _HSV_V(r, g, b)      _MAX2(_MAX2(r, g), b)
    #define _DESAT_CH(c, v, pct) ((c) + (((v) - (c)) * (pct)) / 100)
    #define DESAT_RGB(r, g, b, pct)                                             \
        _DESAT_CH(r, _HSV_V(r, g, b), pct), _DESAT_CH(g, _HSV_V(r, g, b), pct), \
            _DESAT_CH(b, _HSV_V(r, g, b), pct)

    #define RGB_SOLARIZED_BASE03  DESAT_RGB(0, 6, 9, 0)
    #define RGB_SOLARIZED_BASE02  DESAT_RGB(1, 9, 14, 0)
    #define RGB_SOLARIZED_BASE01  DESAT_RGB(25, 40, 45, 0)
    #define RGB_SOLARIZED_BASE00  DESAT_RGB(33, 51, 58, 0)
    #define RGB_SOLARIZED_BASE0   DESAT_RGB(58, 76, 78, 0)
    #define RGB_SOLARIZED_BASE1   DESAT_RGB(74, 91, 91, 0)
    #define RGB_SOLARIZED_BASE2   DESAT_RGB(218, 206, 170, 0)
    #define RGB_SOLARIZED_BASE3   DESAT_RGB(250, 235, 196, 0)
    #define RGB_SOLARIZED_YELLOW  DESAT_RGB(118, 64, 0, 35)
    #define RGB_SOLARIZED_ORANGE  DESAT_RGB(152, 18, 2, 35)
    #define RGB_SOLARIZED_RED     DESAT_RGB(183, 8, 7, 35)
    #define RGB_SOLARIZED_MAGENTA DESAT_RGB(166, 9, 57, 35)
    #define RGB_SOLARIZED_VIOLET  DESAT_RGB(38, 42, 141, 35)
    #define RGB_SOLARIZED_BLUE    DESAT_RGB(5, 66, 164, 35)
    #define RGB_SOLARIZED_CYAN    DESAT_RGB(6, 91, 80, 35)
    #define RGB_SOLARIZED_GREEN   DESAT_RGB(60, 81, 0, 35)
#endif

#define _INIT_RGB(red, green, blue) {.r = red, .g = green, .b = blue}
#define INIT_RGB(...)               _INIT_RGB(__VA_ARGS__)

void override_led_flags(void);
