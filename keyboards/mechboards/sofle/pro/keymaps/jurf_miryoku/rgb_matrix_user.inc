RGB_MATRIX_EFFECT(my_blank_effect)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool my_blank_effect(effect_params_t* params) {
	RGB_MATRIX_USE_LIMITS(led_min, led_max);
	for (uint8_t i = led_min; i < led_max; i++) {
		rgb_matrix_set_color(i, 0x00, 0x00, 0x00);
	}
	return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
