#include "kb.h"

#define _QWERTY 0
#define _LOWER 1
#define _RAISE 2
#define _ADJUST 4

#define LOWER  MO(_LOWER)
#define RAISE  MO(_RAISE)
#define ADJUST MO(_ADJUST)

// enum custom_keycodes {
//   QWERTY = SAFE_RANGE
// };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT_ortho(
    KC_TAB,   KC_Q,       KC_W,       KC_E,       KC_R,   KC_T,   KC_Y,    KC_U,      KC_I,    KC_O,      KC_P, KC_BSLS, KC_BSPC, \
    KC_ESC,   KC_A,       KC_S,       KC_D,       KC_F,   KC_G,   KC_H,    KC_J,      KC_K,    KC_L,      KC_SCLN, KC_QUOT, KC_ENT, \
    KC_LSFT,  KC_Z,       KC_X,       KC_C,       KC_V,   KC_B,   KC_N,    KC_M,      KC_COMM, KC_DOT,    KC_SLSH, KC_UP, KC_ENT, \
    LOWER,   KC_LCTL,    KC_LGUI,    KC_LALT,    LOWER,   KC_SPC, ADJUST, RAISE,   KC_LEFT, KC_DOWN,   KC_RGHT \
), // ADJUST is the 5th key from the left as the plate was shifted

[_LOWER] = LAYOUT_ortho(
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______, KC_DEL, \
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, _______, KC_PIPE, \
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12, S(KC_NUHS), S(KC_NUBS), _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_MPLY \
),

[_RAISE] = LAYOUT_ortho(
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0, _______, KC_BSPC, \
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, _______, KC_BSLS, \
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NUHS, KC_NUBS, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY \
),

[_ADJUST] = LAYOUT_ortho(
  _______, RESET,   _______, AU_ON,   AU_OFF,  AG_NORM, AG_SWAP,  _______, _______, _______ , _______, _______, KC_DEL, \
  _______, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______, _______, \
  _______, BL_TOGG, BL_DEC,  BL_INC,  BL_BRTG, _______, _______, _______, _______, _______, _______, _______, \
  _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_SLCK, KC_PAUS, _______ \
)

};

// layer_state_t layer_state_set_user(layer_state_t state) {
//   return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
// }

void matrix_init_user(void) {
    //set_single_persistent_default_layer(_QWERTY);
}

void matrix_scan_user(void) {
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return true;
}

