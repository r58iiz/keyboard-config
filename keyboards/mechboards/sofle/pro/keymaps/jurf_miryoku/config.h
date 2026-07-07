#pragma once

#undef TAPPING_TERM
#define TAPPING_TERM 160

// Enable rapid switch from tap to hold, disables double tap hold auto-repeat.
#define QUICK_TAP_TERM 0
// Do not wait for tapping term if chord is well-formed
#define PERMISSIVE_HOLD

// Auto Shift
#define NO_AUTO_SHIFT_ALPHA // Cannot be used due to homerow mods
#define NO_AUTO_SHIFT_TAB   // Fixes tabbing in games
#define AUTO_SHIFT_TIMEOUT  TAPPING_TERM
#undef AUTO_SHIFT_NO_SETUP
#define AUTO_SHIFT_NO_SETUP
// #define RETRO_SHIFT 500

// Mouse key speed and acceleration.
#undef MOUSEKEY_DELAY
#define MOUSEKEY_DELAY 0
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 16
#undef MOUSEKEY_WHEEL_DELAY
#define MOUSEKEY_WHEEL_DELAY 0
#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED 6
#undef MOUSEKEY_TIME_TO_MAX
#define MOUSEKEY_TIME_TO_MAX 64

#undef TAPPING_TERM_PER_KEY
#define TAPPING_TERM_PER_KEY
#define TAPPING_TOGGLE 2


// Save space
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_MUSIC_MODE
#define NO_ACTION_ONESHOT
#define LAYER_STATE_16BIT

#define ENCODER_RESOLUTION 4
