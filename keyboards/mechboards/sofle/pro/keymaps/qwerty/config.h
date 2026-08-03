#pragma once

#define TAPPING_TOGGLE 1
#define TAPPING_TERM   200
#define PERMISSIVE_HOLD

#define RGB_MATRIX_MODE_NAME_ENABLE


// #define MK_KINETIC_SPEED

#if defined(MK_KINETIC_SPEED)
// Kinetic
    #define MOUSEKEY_DELAY                       5
    #define MOUSEKEY_INTERVAL                    10
    #define MOUSEKEY_MOVE_DELTA                  16
    #define MOUSEKEY_INITIAL_SPEED               100
    #define MOUSEKEY_BASE_SPEED                  5000
    #define MOUSEKEY_DECELERATED_SPEED           400
    #define MOUSEKEY_ACCELERATED_SPEED           3000
    #define MOUSEKEY_WHEEL_INITIAL_MOVEMENTS     16
    #define MOUSEKEY_WHEEL_BASE_MOVEMENTS        32
    #define MOUSEKEY_WHEEL_ACCELERATED_MOVEMENTS 48
    #define MOUSEKEY_WHEEL_DECELERATED_MOVEMENTS 8
#else
// Accelerated
    #define MOUSEKEY_DELAY             10
    #define MOUSEKEY_INTERVAL          20
    #define MOUSEKEY_MOVE_DELTA        8
    #define MOUSEKEY_MAX_SPEED         2
    #define MOUSEKEY_TIME_TO_MAX       15
    #define MOUSEKEY_WHEEL_DELAY       10
    #define MOUSEKEY_WHEEL_INTERVAL    80
    #define MOUSEKEY_WHEEL_DELTA       1
    #define MOUSEKEY_WHEEL_MAX_SPEED   8
    #define MOUSEKEY_WHEEL_TIME_TO_MAX 40
#endif
