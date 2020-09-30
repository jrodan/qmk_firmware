#include QMK_KEYBOARD_H
#include "lib/lib8tion/lib8tion.h"

#define TAPPING_TERM 350

static uint32_t underglow_mode;
static uint32_t custom_light_mode;

// rgb timeout
// long start = 0;
// bool timer_led = true;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, 
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, 
        KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, 
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, MT(MOD_RSFT, KC_SLSH), KC_UP, KC_SLSH, 
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, MO(1), KC_LEFT, KC_DOWN, KC_RGHT),
    [1] = LAYOUT(
        RESET,   KC_F1,  KC_F2,  KC_F3,       KC_F4,  KC_F5,  KC_F6,  KC_F7,    KC_F8,   KC_F9,  KC_F10,   KC_F11,  KC_F12,  KC_DEL, 
        _______, KC_F14, KC_F15, LCTL(KC_UP), KC_F12, KC_F13, _______, _______, _______, _______, _______, _______, _______, _______, 
        _______, RGB_TOG, RGB_MOD, RGB_HUI,   RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______,         _______, 
        _______,    BL_TOGG, BL_DEC, BL_INC, BL_STEP, _______, _______, _______, _______, _______,      _______, KC__VOLUP, _______, 
        _______, _______, _______,                      _______,                    _______, _______, KC_MUTE, KC__VOLDOWN, _______),
};

void rgb_matrix_layer_helper(uint8_t hue, uint8_t sat, uint8_t val, uint8_t mode, uint8_t speed, uint8_t led_type) {
    HSV hsv = {hue, sat, val};
    if (hsv.v > rgb_matrix_get_val()) {
        hsv.v = rgb_matrix_get_val();
    }

    switch (mode) {
        case 1:  // breathing
        {
            uint16_t time = scale16by8(g_rgb_timer, speed / 8);
            hsv.v         = scale8(abs8(sin8(time) - 128) * 2, hsv.v);
            RGB rgb       = hsv_to_rgb(hsv);
            for (uint8_t i = 0; i < DRIVER_LED_TOTAL; i++) {
                if (HAS_FLAGS(g_led_config.flags[i], led_type)) {
                    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
                }
            }
            break;
        }
        default:  // Solid Color
        {
            RGB rgb = hsv_to_rgb(hsv);
            for (uint8_t i = 0; i < DRIVER_LED_TOTAL; i++) {
                if (HAS_FLAGS(g_led_config.flags[i], led_type)) {
                    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
                }
            }
            break;
        }
    }
}

void set_underglow(void) {
    switch (underglow_mode) {
        case 1: {
            // breath
            rgb_matrix_layer_helper(HSV_MAGENTA, 1, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW);
        } break;
        default: {
            // static
            rgb_matrix_layer_helper(HSV_CYAN, 0, rgb_matrix_config.speed, LED_FLAG_UNDERGLOW);
        } break;
    }
}

void rgb_matrix_indicators_user(void) {
    if(custom_light_mode == 3) {
        set_underglow();
    }
}

void step_underglow(void) {
    if(underglow_mode == 0) {
        underglow_mode = 1;
    } else {
        underglow_mode = 0;
    }
    set_underglow();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    
    // rgb timeout
    // if(start == 0) {
    //     rgb_matrix_enable_noeeprom();
    // }
    // start = timer_read();
    // timer_led = true;

    switch (keycode) {
        case RGB_TOG: // switch rgb
            if (record->event.pressed) {
                /*
                0 = off
                10 = all
                1 = top
                2 = underglow
                3 = custom
                */
                switch (custom_light_mode) {
                    case 10: { // all
                        rgb_matrix_set_flags(LED_FLAG_ALL);
                        step_underglow();
                        rgb_matrix_set_color_all(0, 0, 0);
                        custom_light_mode = 1;
                    } break;
                    case 1: { // top
                        rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER);
                        rgb_matrix_set_color_all(0, 0, 0);
                        custom_light_mode = 2;
                    } break;
                    case 2: { // underglow
                        rgb_matrix_set_flags(LED_FLAG_UNDERGLOW);
                        rgb_matrix_set_color_all(0, 0, 0);
                        custom_light_mode = 0;
                    } break;
                    case 0: { // off
                        rgb_matrix_set_flags(LED_FLAG_NONE);
                        rgb_matrix_disable_noeeprom();
                        custom_light_mode = 3;
                    } break;
                    default: { // custom
                        // default is the custom mode
                        rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER);
                        rgb_matrix_enable_noeeprom();
                        step_underglow(); // set custom underglow
                        custom_light_mode = 10; // set switch to all RGB for next iteration
                    } break;
                }
            }
            return false;
        case BL_TOGG:
            if (record->event.pressed) {
                step_underglow();
            }
            return false;
        default:
            return true;  // Process all other keycodes normally
    }
}

void suspend_power_down_user(void) {
    rgb_matrix_set_suspend_state(true);
    rgb_matrix_set_flags(LED_FLAG_NONE);
    rgb_matrix_disable_noeeprom();
}

void suspend_wakeup_init_user(void) {
    rgb_matrix_set_suspend_state(false);
    rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER);
    rgb_matrix_enable_noeeprom();
    step_underglow(); // set custom underglow
}

// void matrix_scan_user(void) {
//     // rgb timeout
//     if (timer_elapsed(start) > 5000) {
//         start = 0;
//         timer_clear();
//         timer_led = false;
//         rgb_matrix_disable_noeeprom();
//     }
// }

// https://github.com/qmk/qmk_firmware/blob/master/keyboards/crkbd/keymaps/gotham/rgb.c
// https://github.com/qmk/qmk_firmware/blob/a4fd5e2491aa7213d15ef2ff3615b8eb75660e93/keyboards/crkbd/keymaps/rpbaptist/keymap.c
// https://github.com/qmk/qmk_firmware/blob/caa70df816033c30dbbbf4c5a90d803c7bb1dfde/users/curry/rgb_matrix_user.c
// https://github.com/qmk/qmk_firmware/blob/2ffa4798a43682a73598c0b43d46abcca7cd6e83/keyboards/massdrop/alt/keymaps/reywood/keymap.c
// https://github.com/qmk/qmk_firmware/blob/a4fd5e2491aa7213d15ef2ff3615b8eb75660e93/keyboards/crkbd/keymaps/rpbaptist/keymap.c
// https://github.com/qmk/qmk_firmware/blob/caa70df816033c30dbbbf4c5a90d803c7bb1dfde/users/curry/rgb_matrix_user.c
// per layer / per key https://www.reddit.com/r/olkb/comments/92w69w/heres_how_ive_implemented_perlayerperkey_color/

// customizing of RGB effects: https://github.com/SirTimmyTimbit/customizable-gradient-effect-for-drop-alt