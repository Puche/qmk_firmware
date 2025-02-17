/* Copyright 2024 lostdotfish
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
* along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.
*/

#include QMK_KEYBOARD_H

// Time to hold ESC (for change layer) in miliseconds, by default 1000 -> 1 sec
#define ESC_HOLD_TIME 1000 
// Key to perform layer change, please, be sure the key is present in all Layers, or you cannot change Layer.
#define MACRO_KEY_LAYER_CHANGE KC_ESC

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//generic default keymap with Razor defaults (+ 20 shifts layer)

     [0] = LAYOUT(
        KC_ESC, KC_1, KC_2, KC_3, KC_4,
        KC_TAB, KC_Q, KC_W, KC_E, KC_R,
        KC_LSFT, KC_A, KC_S, KC_D, KC_F,
        KC_LCTL, KC_Z, KC_X, KC_C, KC_V,
        KC_LALT, KC_LEFT, KC_RIGHT, KC_DOWN, KC_UP,
        KC_SPACE
    ),

//Second example keymap with all modifier keys replaced with numbers or letters
     
   [1] = LAYOUT(
        KC_ESC, KC_1, KC_2, KC_3, KC_4,
        KC_I, KC_Q, KC_W, KC_E, KC_R,
        KC_J, KC_A, KC_S, KC_D, KC_F,
        KC_K, KC_Z, KC_X, KC_C, KC_V,
        KC_L, KC_LEFT, KC_RIGHT, KC_DOWN, KC_UP,
        KC_SPACE
    ),

//, RGB Contol Keymap
   [2] = LAYOUT(
        KC_ESC, RM_SATU, RM_SATD, RM_TOGG, KC_4,
        KC_TAB, RM_SPDD, RM_NEXT, RM_SPDU, RM_HUEU,
        KC_LSFT, RM_VALD, RM_PREV, RM_VALU, RM_HUED,
        KC_LCTL, KC_Z, KC_X, KC_C, KC_V,
        KC_LALT, KC_LEFT, KC_RIGHT, KC_DOWN, KC_UP,
        KC_SPACE
    ) 

};

layer_state_t layer_state_set_user(layer_state_t state) {
    gpio_write_pin_high(GP23);
    gpio_write_pin_high(GP24);
    gpio_write_pin_high(GP25);
    switch (get_highest_layer(state)) {
        case _DEFAULT:
            gpio_write_pin_low(GP23);
            break;
        case _LETTERS:
            gpio_write_pin_low(GP25);
            break;
        case _CONTROL:
            gpio_write_pin_low(GP24);
            break;
    }
    return state;
}
void suspend_power_down_kb(void) {
    // code will run multiple times while keyboard is suspended
    gpio_write_pin_high(GP23);
    gpio_write_pin_high(GP24);
    gpio_write_pin_high(GP25);
    suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
    layer_state_set_kb(layer_state);
    suspend_wakeup_init_user();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t esc_hold_timer = 0;
    static bool esc_held = false; 

    if (keycode == MACRO_KEY_LAYER_CHANGE) {
        if (record->event.pressed) {
            esc_hold_timer = timer_read();  
            esc_held = false;  
        } else {
            uint16_t held_time = timer_elapsed(esc_hold_timer);  

            if (held_time >= ESC_HOLD_TIME) {
                switch (get_highest_layer(layer_state)) {
                    case 0:
                        layer_move(1);
                        break;
                    case 1:
                        layer_move(2);
                        break;
                    case 2:
                        layer_move(0);
                        break;
                }
                esc_held = true; 
            }
            if (!esc_held) {
                tap_code(KC_ESC);  
            }
        }
        return false;  
    }
    return true;
}
