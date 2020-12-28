##include "bm60rgb.h"
#include "bm60rgb.c"

#include <math.h>
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

/*---------------------------------  Outrun  ---------------------------------*/

static void rgb_matrix_outrun(uint16_t led_i, bool init) {
  const rgb_led led = g_rgb_leds[led_i];

  const uint8_t hue = (led.matrix_co.raw != 0xFF)
    ? 0 + 10 * led.matrix_co.row
    : 32;

  HSV hsv = {
    hue,
    0xFF,
    rgb_matrix_config.hsv.v,
  };
  RGB rgb = hsv_to_rgb(hsv);

  const float vpct = rgb_matrix_config.hsv.v / 255.f;
  const RGB highlight = {
    0x00 * vpct,
    0xFF * vpct,
    0x00 * vpct,
  };

  const uint8_t cuttoff = 200; // 0-255, larger = slower the fade
  if (g_key_hit[led_i] <= cuttoff) {
    float pct = (float)g_key_hit[led_i] / (float)cuttoff;
    rgb.r = highlight.r - pct * (highlight.r - rgb.r);
    rgb.g = highlight.g - pct * (highlight.g - rgb.g);
    rgb.b = highlight.b - pct * (highlight.b - rgb.b);
  }

  rgb_matrix_set_color(led_i, rgb.r, rgb.g, rgb.b);
}

/*----------------------------------  Snake  ---------------------------------*/

// Tagged snake_cell union
typedef enum { SNKC_EMPTY_T = 0, SNKC_BODY_T, SNKC_APPLE_T } snk_cell_tag_t;
typedef struct {
  union {
                 // Empty
    uint8_t seg; // Body
                 // Apple
  };
  snk_cell_tag_t tag;
} snk_cell_t;
#define SNK_EMPTY      (snk_cell_t){{0},   SNKC_EMPTY_T}
#define SNK_BODY(seg)  (snk_cell_t){{seg}, SNKC_BODY_T}
#define SNK_APPLE      (snk_cell_t){{0},   SNKC_APPLE_T}

typedef struct { uint8_t row, col; } snk_pos_t;

// Snake game state
static struct {
  uint32_t last_tick;

  snk_pos_t delta;
  uint8_t   len;
  snk_pos_t body[67];
  snk_pos_t apple;

  snk_cell_t led_states[ISSI3733_LED_COUNT];
} snk = {0};

static void snk_init(void) {
  snk.last_tick = g_tick;

  snk.delta = (snk_pos_t){0, 1}; // start going right
  snk.len = 1;
  snk.body[0] = (snk_pos_t){2, 2}; // start from the S key ;)
  snk.apple = (snk_pos_t){2, 5}; // spawn apple to the right

  for (int i = 0; i < ISSI3733_LED_COUNT; i++)
    snk.led_states[i] = SNK_EMPTY;
}

// #define __ 255
// static const uint8_t KEY_TO_LED_MAP[MATRIX_ROWS][MATRIX_COLS] = {
//   { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14},
//   {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29},
//   {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, __, 42, 43},
//   {44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, __, 56, 57},
//   {58, 59, 60, __, __, __, 61, __, __, __, 62, 63, 64, 65, 66},
// };

// less efficient than a LUT, but generalizes across keyboards
static uint8_t key_to_led(uint8_t row, uint8_t col) {
  for (uint8_t i = 0; i < DRIVER_LED_TOTAL; i++) {
    const rgb_led led = g_rgb_leds[i];
    if (led.matrix_co.row == row && led.matrix_co.col == col)
      return i;
  }
  return 255;
}

static void snk_update_state(void) {
  // update movement direction
  static uint16_t last_keycode = 0;
  // prevent reversing into yourself
  if (!(
    (g_last_keycode == KC_UP   && last_keycode == KC_DOWN) ||
    (g_last_keycode == KC_DOWN && last_keycode == KC_UP)   ||
    (g_last_keycode == KC_LEFT && last_keycode == KC_RGHT) ||
    (g_last_keycode == KC_RGHT && last_keycode == KC_LEFT)
  )) {
    switch (g_last_keycode) {
    // Snake direction
    case KC_UP:   snk.delta = (snk_pos_t){-1,  0}; break;
    case KC_LEFT: snk.delta = (snk_pos_t){ 0, -1}; break;
    case KC_DOWN: snk.delta = (snk_pos_t){ 1,  0}; break;
    case KC_RGHT: snk.delta = (snk_pos_t){ 0,  1}; break;
    // Exit snake mode
    case KC_ESC:  rgb_matrix_step(); break;
    default: break;
    }
    last_keycode = g_last_keycode;
  }

  // make room for new head
  for (int i = snk.len - 1; i >= 0; i--)
    snk.body[i + 1] = snk.body[i];

  // move snake's head
  snk.body[0].row += snk.delta.row;
  snk.body[0].col += snk.delta.col;

  // wrap snake around edges
  if (snk.body[0].row == 255)         snk.body[0].row = MATRIX_ROWS - 1;
  if (snk.body[0].col == 255)         snk.body[0].col = MATRIX_COLS - 1;
  if (snk.body[0].row == MATRIX_ROWS) snk.body[0].row = 0;
  if (snk.body[0].col == MATRIX_COLS) snk.body[0].col = 0;

  // check for self-collision
  for (int i = 1; i < snk.len; i++) {
    if (snk.body[i].row == snk.body[0].row &&
        snk.body[i].col == snk.body[0].col) {
      // TODO: add a nice game-over animation
      rgb_matrix_step();
      return;
    }
  }

  // check for apple collision
  if (snk.apple.row == snk.body[0].row &&
      snk.apple.col == snk.body[0].col) {
    snk.len++;
    // spawn new apple
    do {
      snk.apple.row = rand() % MATRIX_ROWS;
      snk.apple.col = rand() % MATRIX_COLS;
    } while (key_to_led(snk.apple.row, snk.apple.col) == 255);
  }

  // update LEDs
  for (int i = 0; i < ISSI3733_LED_COUNT; i++)
    snk.led_states[i] = SNK_EMPTY;

  for (int i = 0; i < snk.len; i++) {
    const uint16_t led_i = key_to_led(snk.body[i].row, snk.body[i].col);
    if (led_i != 255)
      snk.led_states[led_i] = SNK_BODY(i);
  }

  snk.led_states[key_to_led(snk.apple.row, snk.apple.col)] = SNK_APPLE;
}

static void snk_run(uint16_t led_i) {
  // check if it's time to run a game state update
  const uint16_t speed = max(10, 30 - snk.len * 2); // <-- tweak difficulty
  if (g_tick - snk.last_tick > speed) {
    snk.last_tick = g_tick;
    snk_update_state();
  }

  // update leds based off game state
  const snk_cell_t cell = snk.led_states[led_i];

  RGB rgb;
  switch(cell.tag) {
  case SNKC_APPLE_T:
    rgb = (RGB){ 0, 255, 0 };
    break;
  case SNKC_BODY_T:
    rgb = (RGB){
      sin(cell.seg * 0.5 + 0) * 127 + 128,
      sin(cell.seg * 0.5 + 2) * 127 + 128,
      sin(cell.seg * 0.5 + 4) * 127 + 128
    };
    break;
  case SNKC_EMPTY_T:
  default:
    rgb = (RGB){ 0, 0, 0 };
    break;
  }

  rgb_matrix_set_color(led_i, rgb.r, rgb.g, rgb.b);
}

static void rgb_matrix_snake(uint16_t led_i, bool init) {
  if (init) snk_init();
  else snk_run(led_i);
}

/*---------------------------------  Export  ---------------------------------*/

const rgb_matrix_effect_f rgb_matrix_effects_user[] = {
    rgb_matrix_outrun,
    rgb_matrix_snake,
};

// override null-defns in rgb_matrix.c
const uint8_t rgb_matrix_effects_user_count = sizeof(rgb_matrix_effects_user) / sizeof(rgb_matrix_effects_user[0]);