ifeq ($(strip $(OLED_ENABLE)), yes)
    ifneq ($(wildcard $(KEYMAP_PATH)/display_oled.c),)
        SRC += $(KEYMAP_PATH)/display_oled.c
    else
        SRC += keyboards/mechboards/common/display_oled.c
    endif
endif

