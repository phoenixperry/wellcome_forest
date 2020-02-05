#include "AsyncDelay.h"
#include "tree.h"
#include "util.h"

CRGB leds[LED_COUNT];
uint16_t time = 0; // timer used for animations
AsyncDelay asyncDelay;

/**
 * Tree object
 * contains the state of the neopixel and button
 * has an id string, from "C" - "J" (C is the initial beacon tree)
 *
 */
Tree tree("C");

void setup() {
  // setup Serial
  Serial.begin(9600);
  // setup neopixel
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  // setup button
  pinMode(BUTTON_LED, OUTPUT);
  pinMode(BUTTON_IN, INPUT);
  pinMode(BUTTON_OUT, OUTPUT);
  digitalWrite(BUTTON_OUT, HIGH);
  digitalWrite(BUTTON_LED, HIGH);
  // set initial hue
  tree.hue = tree.is_beacon() ? 20.0 : 100.0;
}

void loop() {
  time++;
  if (tree.state == idle) {
    double speed = 0.002;
    tree.hue = 100.0;
    tree.saturation = 150 + abs(sin(time * speed)) * 50;
    tree.value = 255 - abs(cos(time * speed)) * 55;
    tree.on_pressed([] {
      asyncDelay.start(2000, AsyncDelay::MILLIS);
      Serial.println("on_pressed");
    });
    tree.while_pressed([] {
      double speed = 0.01;
      tree.value = 255 - abs(cos(time * speed)) * 55;
      if (asyncDelay.isExpired()) {
        tree.reset_button();
      }
    });
    tree.show();
  } else if (tree.state == beacon) {
    tree.hue = -sin(time * 0.007) * 30 + 50;
    tree.value = 220 + sin(time * 0.007) * 35;
    tree.saturation = 255;
    tree.on_pressed([] { Serial.println("lol"); });
    tree.while_pressed([] {
      if (tree.hue > 20) {
        tree.hue -= 0.2;
      } else {
        tree.hue = 20.;
      }
    });
    tree.show();
  }

  else if (tree.state == fail) {
    tree.set_color(240, 240, 200);
  }
}

// void loop() {

//   /*--- PARSE STATE TRANSITIONS ---*/
//   while (Serial.available()) {
//     String msg = Serial.readStringUntil('\n');
//     if (is_valid_msg(msg)) {
//       String id = get_tree_id(msg);
//       if (tree.matches(id)) {
//         Serial.println("you selected this tree!");
//       }
//     }
//   }

//   time++;
//   /*--- IDLE STATE ---*/
//   if (current_state == idle) {
//     double speed = 0.002;
//     double saturation = 150 + abs(sin(time * speed)) * 50;
//     double value = 255 - abs(cos(time * speed)) * 55;
//     if (is_button_pressed()) {
//       button_was_pressed = true;
//       asyncDelay.start(3000, AsyncDelay::MILLIS);
//       time = 0;
//     }
//     if (button_was_pressed) {
//       saturation = 255 - time * 0.05;
//       if (asyncDelay.isExpired()) {
//         button_was_pressed = false;
//       }
//     }
//     set_hsv(hue, saturation, value);
//   }
//   /*--- BEACON STATE ---*/
//   else if (current_state == beacon) {
//     if (!button_was_pressed) {
//       // beacon animation
//       // handle button press
//       if (is_button_pressed()) {
//         Serial.println(tree_id + " pressed");
//         button_was_pressed = true;
//       }
//     } else {
//       // smoothly reset to orange
//       hue = hue > 20 ? hue - 0.2 : 20.;
//       set_hsv(hue, 255, 255);
//     }
//   }
//   /*--- WAITING STATE ---*/
//   if (current_state == waiting) {
//   }
//   /*--- FAIL STATE ---*/
//   if (current_state == fail) {
//     set_hsv(hue, 240, 200);
//   }
//   /*--- WIN STATE ---*/
//   /*--- WIN_GLOBAL STATE ---*/
// }
