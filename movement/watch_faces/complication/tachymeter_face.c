/*
 * MIT License
 *
 * Copyright (c) 2022 <#author_name#>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include "tachymeter_face.h"

void tachymeter_face_setup(movement_settings_t *settings, uint8_t watch_face_index, void ** context_ptr) {
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(tachymeter_state_t));
        memset(*context_ptr, 0, sizeof(tachymeter_state_t));
        // Do any one-time tasks in here; the inside of this conditional happens only at boot.
    }
    // Do any pin or peripheral setup here; this will be called whenever the watch wakes from deep sleep.
}

void tachymeter_face_activate(movement_settings_t *settings, void *context) {
    tachymeter_state_t *state = (tachymeter_state_t *)context;
    state->active = false;
    // Handle any tasks related to your watch face coming on screen.
}

static void _blink_face_update_lcd(tachymeter_state_t *state){
    char buf[11];
    const char colors[][7]= {" red  ", " Green", " Yellow"};
    sprintf(buf, "BL %c%s", state->fast ? 'F' : 'S', colors[state->color]);
    watch_display_string(buf, 0);
}

bool tachymeter_face_loop(movement_event_t event, movement_settings_t *settings, void *context) {
    tachymeter_state_t *state = (tachymeter_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            // Show your initial UI here.
            _blink_face_update_lcd(state);
            break;
        case EVENT_TICK:
            // If needed, update your display here.
            if (state->active){
                if (event.subsecond % 2 == 0) watch_set_led_off();
                else if (state->color == 0) watch_set_led_red();
                else if (state->color == 1) watch_set_led_green();
                else watch_set_led_yellow();
            }
            break;
        case EVENT_MODE_BUTTON_UP:
            // You shouldn't need to change this case; Mode almost always moves to the next watch face.
            movement_move_to_next_face();
            break;
        case EVENT_LIGHT_BUTTON_UP:
            // If you have other uses for the Light button, you can opt not to illuminate the LED for this event.
            // movement_illuminate_led();
            if (!state->active){
                state->color = (state->color + 1) % 3;
                _blink_face_update_lcd(state);
            }
            break;
        case EVENT_ALARM_BUTTON_UP:
            // Just in case you have need for another button.
            if (!state->active){
                state->active = true;
                watch_clear_display();
                movement_request_tick_frequency(state->fast ? 8 : 2);
            } else {
                state->active = false;
                watch_set_led_off();
                _blink_face_update_lcd(state);
            }
            break;
        case EVENT_ALARM_LONG_PRESS:
            if (!state->active){
                state->fast = !state->fast;
                _blink_face_update_lcd(state);
            }
            break;
        case EVENT_TIMEOUT:
            // Your watch face will receive this event after a period of inactivity. If it makes sense to resign,
            // you may uncomment this line to move back to the first watch face in the list:
            // movement_move_to_face(0);
            break;
        case EVENT_LOW_ENERGY_UPDATE:
            // If you did not resign in EVENT_TIMEOUT, you can use this event to update the display once a minute.
            // Avoid displaying fast-updating values like seconds, since the display won't update again for 60 seconds.
            // You should also consider starting the tick animation, to show the wearer that this is sleep mode:
            // watch_start_tick_animation(500);
            break;
        default:
            break;
    }

    // return true if the watch can enter standby mode. If you are PWM'ing an LED or buzzing the buzzer here,
    // you should return false since the PWM driver does not operate in standby mode.
    return true;
}

void tachymeter_face_resign(movement_settings_t *settings, void *context) {
    watch_set_led_off();
    // handle any cleanup before your watch face goes off-screen.
}
