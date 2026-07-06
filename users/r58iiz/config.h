#pragma once

#ifdef VIAL_ENABLE
    #define VIAL_KEYBOARD_UID      {0xCF, 0x28, 0x94, 0xAF, 0x53, 0x24, 0xDD, 0x89}
    #define VIAL_UNLOCK_COMBO_ROWS {0, 0}
    #define VIAL_UNLOCK_COMBO_COLS {0, 1}
#endif

// Personal
#define OLED_TIMEOUT               0
#define OLED_BRIGHTNESS            60
#define CUSTOM_OLED_TIMEOUT        15000

#define SPLIT_TRANSACTION_IDS_USER RPC_SPLIT_SYNC

#define SPLIT_LAYER_STATE_ENABLE