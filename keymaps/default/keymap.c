#include QMK_KEYBOARD_H
#include "keymap_italian.h"
#include "quantum.h"
#include "gpio.h"

// Custom Layers
enum layer_number {
    _BASE,
    _FN,
};

// Custom keycodes
enum custom_keycodes {
    BOOTLOADER = SAFE_RANGE,
    EEPROMRESET,
};

#define KC_FN MO(_FN)

// BOOTLOADER timer
static uint16_t bootloader_timer = 0;
static bool bootloader_hold = false;

// EEPROMRESET timer
static uint16_t eepromreset_timer = 0;
static bool eepromreset_hold = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_ESCAPE, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,  KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_INS,  KC_HOME,
        KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,  KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_DEL,  KC_END,
        KC_CAPS,   KC_A,    KC_S,    KC_D,    KC_F,    KC_G,  KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_NUHS, KC_ENT,  KC_PAUS, KC_PGUP,
        KC_LSFT,   KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,  KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_PGDN,
        KC_LCTL,   KC_LGUI, KC_LALT, KC_SPACE,KC_RALT, KC_FN, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT),

    [_FN] = LAYOUT(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,      KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_TRNS, KC_TRNS, RGB_MOD,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_TOG, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_VAI, KC_TRNS,
        BOOTLOADER, KC_TRNS, KC_TRNS, EEPROMRESET, KC_TRNS, KC_TRNS, KC_TRNS, RGB_SPD, RGB_VAD, RGB_SPI),
};

//------------------------------------------------------------//
// 3KRO enforcement
static uint8_t keys_held = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode < SAFE_RANGE && !IS_MOD(keycode)) {
        if (record->event.pressed) {
            if (keys_held >= 3) return false;
            keys_held++;
        } else {
            if (keys_held > 0) keys_held--;
        }
    }

    switch (keycode) {
        case BOOTLOADER:
            if (record->event.pressed) {
                bootloader_timer = timer_read();
                bootloader_hold = true;
                dprintf("Bootloader key pressed, timer started\n");
            } else {
                bootloader_hold = false;
                dprintf("Bootloader key released\n");
            }
            return false;
        case EEPROMRESET:
            if (record->event.pressed) {
                eepromreset_timer = timer_read();
                eepromreset_hold = true;
                dprintf("EEPROM reset key pressed, timer started\n");
            } else {
                eepromreset_hold = false;
                dprintf("EEPROM reset key released\n");
            }
            return false;
        default:
            return true;
    }
}

//----------------------------------------------------//
void matrix_scan_user(void) {
    if (bootloader_hold) {
        if (timer_elapsed(bootloader_timer) > 3000) {
            bootloader_hold = false;
            dprintf("3 seconds elapsed - entering bootloader\n");
            reset_keyboard();
        }
    }
    if (eepromreset_hold) {
        if (timer_elapsed(eepromreset_timer) > 3000) {
            eepromreset_hold = false;
            dprintf("3 seconds elapsed - resetting EEPROM\n");
            eeconfig_init();
        }
    }
}

//--------------------------------------------------------//
	//LED sleep
void suspend_power_down_user(void) {
    rgb_matrix_set_suspend_state(true);
}
void suspend_wakeup_init_user(void) {
    rgb_matrix_set_suspend_state(false);
}

//-----------------------------------------------------------//
void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(21, 255, 128); // orange
}

//-----------------------------------------------------------------------------//
void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Caps Lock indicator
    if (host_keyboard_led_state().caps_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(31, 255, 0, 0);
    }
    // FN layer indicators
    if (get_highest_layer(layer_state) == 1) {
        // FN-keys (Blue)
        RGB_MATRIX_INDICATOR_SET_COLOR(0,  0, 100, 255); // GRV
        RGB_MATRIX_INDICATOR_SET_COLOR(67, 0, 100, 255); // FN-KEY
        RGB_MATRIX_INDICATOR_SET_COLOR(1,  0, 100, 255); // F1
        RGB_MATRIX_INDICATOR_SET_COLOR(2,  0, 100, 255); // F2
        RGB_MATRIX_INDICATOR_SET_COLOR(3,  0, 100, 255); // F3
        RGB_MATRIX_INDICATOR_SET_COLOR(4,  0, 100, 255); // F4
        RGB_MATRIX_INDICATOR_SET_COLOR(5,  0, 100, 255); // F5
        RGB_MATRIX_INDICATOR_SET_COLOR(6,  0, 100, 255); // F6
        RGB_MATRIX_INDICATOR_SET_COLOR(7,  0, 100, 255); // F7
        RGB_MATRIX_INDICATOR_SET_COLOR(8,  0, 100, 255); // F8
        RGB_MATRIX_INDICATOR_SET_COLOR(9,  0, 100, 255); // F9
        RGB_MATRIX_INDICATOR_SET_COLOR(10, 0, 100, 255); // F10
        RGB_MATRIX_INDICATOR_SET_COLOR(11, 0, 100, 255); // F11
        RGB_MATRIX_INDICATOR_SET_COLOR(12, 0, 100, 255); // F12
        // RGB controls (Green)
        RGB_MATRIX_INDICATOR_SET_COLOR(29, 0, 255, 0); // RGB_TOG
        RGB_MATRIX_INDICATOR_SET_COLOR(15, 0, 255, 0); // RGB_MOD
        RGB_MATRIX_INDICATOR_SET_COLOR(60, 0, 255, 0); // RGB_VAI
        RGB_MATRIX_INDICATOR_SET_COLOR(70, 0, 255, 0); // RGB_VAD
        RGB_MATRIX_INDICATOR_SET_COLOR(71, 0, 255, 0); // RGB_SPI
        RGB_MATRIX_INDICATOR_SET_COLOR(69, 0, 255, 0); // RGB_SPD
        // Bootloader and EEpromreset (Red)
        RGB_MATRIX_INDICATOR_SET_COLOR(62, 255, 0, 0); // BOOTLOADER
        RGB_MATRIX_INDICATOR_SET_COLOR(65, 255, 0, 0); // EEPROMRESET
    }
}
