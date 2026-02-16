#include QMK_KEYBOARD_H
#include "keymap_italian.h"
#include "quantum.h"
#include "gpio.h" // pulls in gpio.h, led.h, etc.

enum layer_number {
  _BASE,
  _FN,
};

enum custom_keycodes {
BOOTLOADER = SAFE_RANGE,
EEPROMRESET,
/*MACOS*/
/*TOGGLERGB*/
};

#define KC_FN MO(_FN)

// Bootloader timer
static uint16_t bootloader_timer = 0;
static bool bootloader_hold = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	[_BASE] = LAYOUT(
		KC_ESCAPE,KC_1,     KC_2,   KC_3,    KC_4,    KC_5,  KC_6,    KC_7,    KC_8,   KC_9,   KC_0,    KC_MINS, KC_EQL, KC_BSPC,KC_INS, KC_HOME,
		KC_TAB,   KC_Q,     KC_W,   KC_E,    KC_R,    KC_T,  KC_Y,    KC_U,    KC_I,   KC_O,   KC_P,    KC_LBRC, KC_RBRC,KC_DEL, KC_END,
		KC_CAPS,  KC_A,     KC_S,   KC_D,    KC_F,    KC_G,  KC_H,    KC_J,    KC_K,   KC_L,   KC_SCLN, KC_QUOT, KC_NUHS,KC_ENT, KC_PAUS, KC_PGUP,
		KC_LSFT,  KC_NUBS,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_N,    KC_M,   KC_COMM,KC_DOT,  KC_SLSH, KC_RSFT,KC_UP,  KC_PGDN,
		KC_LCTL,  KC_LGUI,  KC_LALT,KC_SPACE,KC_RALT, KC_FN, KC_RCTL, KC_LEFT, KC_DOWN,KC_RGHT),
		
	[_FN] = LAYOUT(
		KC_GRV, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11,KC_F12,KC_TRNS, KC_TRNS, RGB_MOD,
		KC_TRNS/*UNICODE_MODE_MACOS*/, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,RGB_TOG,KC_TRNS/*RGB_MATRIX_HUE_UP*/,
		KC_TRNS,KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,RGB_VAI,KC_TRNS,
		BOOTLOADER, KC_TRNS, KC_TRNS,EEPROMRESET, KC_TRNS, KC_TRNS, KC_TRNS, RGB_SPD ,RGB_VAD, RGB_SPI),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode){
        /* #QK_BOOTLOADER# 3-second timer */
        case BOOTLOADER:
            if (record->event.pressed) {
                // Key pressed - start the timer
                bootloader_timer = timer_read();
                bootloader_hold = true;
                dprintf("Bootloader key pressed, timer started\n");
            } else {
                // Key released - cancel if released early
                bootloader_hold = false;
                dprintf("Bootloader key released\n");
            }
            return false;
        /* #QK_REBOOT#*/
        case EEPROMRESET:
            if (!record->event.pressed) {
                return false;
            }
            eeconfig_init();
            return false;
        default:
            return true;
    }
}

// Initialize keyboard - set to solid color mode on boot
void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();                         // Enable RGB
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);     // Set to solid color
    rgb_matrix_sethsv_noeeprom(21, 255, 128);             //Orange color at medium brightness
}

// Caps Lock indicator - lights up Caps Lock key when active
void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR  (31, 255, 0, 0);  // Set Caps Lock key to RED
    }

    // Change indicators when Fn layer is active, every line is a key
    if (get_highest_layer(layer_state) == 1) {
	// FN-keys (Blue)
        RGB_MATRIX_INDICATOR_SET_COLOR  (0, 0, 100, 255);   // GRV
	RGB_MATRIX_INDICATOR_SET_COLOR  (67, 0, 100, 255);  // FN-KEY
        RGB_MATRIX_INDICATOR_SET_COLOR  (1, 0, 100, 255);   // F1
        RGB_MATRIX_INDICATOR_SET_COLOR  (2, 0, 100, 255);   // F2
        RGB_MATRIX_INDICATOR_SET_COLOR  (3, 0, 100, 255);   // F3
        RGB_MATRIX_INDICATOR_SET_COLOR  (4, 0, 100, 255);   // F4
        RGB_MATRIX_INDICATOR_SET_COLOR  (5, 0, 100, 255);   // F5
        RGB_MATRIX_INDICATOR_SET_COLOR  (6, 0, 100, 255);   // F6
        RGB_MATRIX_INDICATOR_SET_COLOR  (7, 0, 100, 255);   // F7
        RGB_MATRIX_INDICATOR_SET_COLOR  (8, 0, 100, 255);   // F8
        RGB_MATRIX_INDICATOR_SET_COLOR  (9, 0, 100, 255);   // F9
        RGB_MATRIX_INDICATOR_SET_COLOR  (10, 0, 100, 255);  // F10
        RGB_MATRIX_INDICATOR_SET_COLOR  (11, 0, 100, 255);  // F11
        RGB_MATRIX_INDICATOR_SET_COLOR  (12, 0, 100, 255);  // F12
        // RGB controls (Green)
        RGB_MATRIX_INDICATOR_SET_COLOR  (29, 0, 255, 0);    // RGB_TOG
        RGB_MATRIX_INDICATOR_SET_COLOR  (15, 0, 255, 0);    // RGB_MOD
        RGB_MATRIX_INDICATOR_SET_COLOR  (60, 0, 255, 0);    // RGB_VAI
        RGB_MATRIX_INDICATOR_SET_COLOR  (70, 0, 255, 0);    // RGB_VAD
        RGB_MATRIX_INDICATOR_SET_COLOR  (71, 0, 255, 0);    // RGB_SPI
        RGB_MATRIX_INDICATOR_SET_COLOR  (69, 0, 255, 0);    // RGB_SPD
	//Bootloader and EEpromreset (Red)
	RGB_MATRIX_INDICATOR_SET_COLOR  (62, 255, 0, 0);	// BOOTLOADER
	RGB_MATRIX_INDICATOR_SET_COLOR  (65, 255, 0, 0);	// EEPROMRESET
    }
}

// Function to check the timer every scan
void matrix_scan_user(void) {
    if (bootloader_hold) {
        uint16_t elapsed = timer_elapsed(bootloader_timer);
        if (elapsed > 3000) {  // 3000ms = 3 seconds
            bootloader_hold = false;
            dprintf("3 seconds elapsed - entering bootloader\n");
            reset_keyboard();
        } else if (elapsed % 1000 == 0 && elapsed > 0) {
            // Debug output every second
            dprintf("Holding for bootloader: %d seconds\n", elapsed / 1000);
        }
    }
}
