# Personal
OLED_ENABLE = yes

# Harden
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
MIDI_ENABLE = no
MUSIC_ENABLE = no

# KEY_OVERRIDE_ENABLE = no

SRC += r58iiz.c
SRC += state/split_state.c
SRC += display/oled_master.c
SRC += display/oled_slave.c
SRC += display/oled_utils.c
SRC += display/oled_images.c
SRC += display/oledWidget/hsv_widget.c
SRC += display/oledWidget/keylogger_widget.c
SRC += display/oledWidget/layer_widget.c
SRC += display/oledWidget/rgbm_widget.c
SRC += display/oledWidget/system_widget.c
SRC += display/oledWidget/wpm_widget.c
