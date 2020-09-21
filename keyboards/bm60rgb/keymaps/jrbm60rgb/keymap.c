/* Copyright 2020 MechMerlin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#define TAPPING_TERM 350

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, 
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, 
        KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, 
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, MT(MOD_RSFT, KC_SLSH), KC_UP, KC_SLSH, 
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, MO(1), KC_LEFT, KC_DOWN, KC_RGHT),
    [1] = LAYOUT(
        RESET, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_DEL, 
        _______, KC_F14, KC_F15, LCTL(KC_UP), KC_F12, KC_F13, _______, _______, _______, _______, _______, _______, _______, _______, 
        _______, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______,         _______, 
        _______,    BL_TOGG, BL_DEC, BL_INC, BL_STEP, _______, _______, _______, _______, _______,      _______, KC__VOLUP, _______, 
        _______, _______, _______,                   _______,                    _______, _______, KC_MUTE, KC__VOLDOWN, _______),

};// This runs every matrix scan (every 'frame')
// void rgb_matrix_indicators_user(void) {
//     led_flags_t flags = rgb_matrix_get_flags();

//     // If we're in either keylight or underglow modes (but not both simultaneously)
//     if (HAS_FLAGS(flags, LED_FLAG_KEYLIGHT) != HAS_FLAGS(flags, LED_FLAG_UNDERGLOW)) {
//         rgb_matrix_set_color(32, 0, 0, 0);
//     }
// }

// void keyboard_post_init_user(void) {
//     // Enable the LED layers
//     rgblight_layers = my_rgb_layers;
// }

// void rgb_matrix_layer_helper(uint8_t red, uint8_t green, uint8_t blue) {
//     for (int i = 0; i < DRIVER_LED_TOTAL; i++) {
//         if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_MODIFIER)) {
//             rgb_matrix_set_color(i, red, green, blue);
//         }
//     }
// }

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // static uint32_t key_timer;

    switch (keycode) {
        /*
        g_led_config
        need a fix to control underglow separte
        RGB_TOG + X = for underglow
        RGB_TOG + Y
        */
        case RGB_TOG:
            if (record->event.pressed) {
                switch (rgb_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER);
                        rgb_matrix_set_color_all(0, 0, 0);
                    } break;
                    case LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER: {
                        rgb_matrix_set_flags(LED_FLAG_UNDERGLOW);
                        rgb_matrix_set_color_all(0, 0, 0);
                    } break;
                    case LED_FLAG_UNDERGLOW: {
                        rgb_matrix_set_flags(LED_FLAG_NONE);
                        rgb_matrix_disable_noeeprom();
                    } break;
                    default: {
                        rgb_matrix_set_flags(LED_FLAG_ALL);
                        rgb_matrix_enable_noeeprom();
                    } break;
                }
            }
            return false;
        default:
            return true;  // Process all other keycodes normally
    }
}

// https://github.com/qmk/qmk_firmware/blob/master/keyboards/crkbd/keymaps/gotham/rgb.c
// https://github.com/qmk/qmk_firmware/blob/a4fd5e2491aa7213d15ef2ff3615b8eb75660e93/keyboards/crkbd/keymaps/rpbaptist/keymap.c
// https://github.com/qmk/qmk_firmware/blob/caa70df816033c30dbbbf4c5a90d803c7bb1dfde/users/curry/rgb_matrix_user.c
// https://github.com/qmk/qmk_firmware/blob/2ffa4798a43682a73598c0b43d46abcca7cd6e83/keyboards/massdrop/alt/keymaps/reywood/keymap.c
// https://github.com/qmk/qmk_firmware/blob/a4fd5e2491aa7213d15ef2ff3615b8eb75660e93/keyboards/crkbd/keymaps/rpbaptist/keymap.c
// https://github.com/qmk/qmk_firmware/blob/caa70df816033c30dbbbf4c5a90d803c7bb1dfde/users/curry/rgb_matrix_user.c
// per layer / per key https://www.reddit.com/r/olkb/comments/92w69w/heres_how_ive_implemented_perlayerperkey_color/

// void rgb_matrix_layer_helper(uint8_t hue, uint8_t sat, uint8_t val, uint8_t mode, uint8_t speed, uint8_t led_type) {
//     HSV hsv = {hue, sat, val};
//     if (hsv.v > rgb_matrix_config.hsv.v) {
//         hsv.v = rgb_matrix_config.hsv.v;
//     }

//     switch (mode) {
//         case 1:  // breathing
//         {
//             uint16_t time = scale16by8(g_rgb_counters.tick, speed / 8);
//             hsv.v         = scale8(abs8(sin8(time) - 128) * 2, hsv.v);
//             RGB rgb       = hsv_to_rgb(hsv);
//             for (uint8_t i = 0; i < DRIVER_LED_TOTAL; i++) {
//                 if (HAS_FLAGS(g_led_config.flags[i], led_type)) {
//                     rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
//                 }
//             }
//             break;
//         }
//         default:  // Solid Color
//         {
//             RGB rgb = hsv_to_rgb(hsv);
//             for (uint8_t i = 0; i < DRIVER_LED_TOTAL; i++) {
//                 if (HAS_FLAGS(g_led_config.flags[i], led_type)) {
//                     rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
//                 }
//             }
//             break;
//         }
//     }
// }