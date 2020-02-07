#include "AsyncDelay.h"
#include "tree.h"
#include "util.h"

CRGB leds[LED_COUNT];
uint16_t time = 0; // timer used for animations
AsyncDelay idle_delay;
AsyncDelay send_state_timer;
int global_speed = 1;

/**
 * Tree object
 * contains the state of the neopixel and button
 * has an id char, from 'C' - 'J' (C is the initial beacon tree)
 *
 */
Tree tree('C');

void setup() {
  // setup Serial
  Serial.begin(9600);
  Serial1.begin(9600);
  // setup neopixel
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  // setup button
  pinMode(BUTTON_LED, OUTPUT);
  pinMode(BUTTON_IN, INPUT);
  pinMode(BUTTON_OUT, OUTPUT);
  digitalWrite(BUTTON_OUT, HIGH);
  digitalWrite(BUTTON_LED, HIGH);
  // set initial hue
  send_state_timer.start(100, AsyncDelay::MILLIS);
}

void send_state(Tree tree) {
  bool local_win = tree.state == TreeState::beacon && tree.button_was_pressed ||
                   tree.state == TreeState::pressed_beacon;
  Serial1.print("{");
  Serial1.print(tree.id);
  Serial1.print(local_win);
  Serial1.print(tree.button_was_pressed);
  Serial1.println("}");

  // debugging
  Serial.print("{");
  Serial.print(tree.id);
  Serial.print(local_win);
  Serial.print(tree.button_was_pressed);
  Serial.println("}");
}

enum GameState { start, playing, won, lost };

void loop() {
  time++;

  if (send_state_timer.isExpired()) {
    send_state(tree);
    send_state_timer.repeat();
  }

  /*** PARSE STATE TRANSITIONS ***/
  while (Serial1.available()) {
    String msg = Serial1.readStringUntil('\n');
    msg.trim();
    if (is_valid_msg(msg)) {
      Serial.println("recieved valid message");
      int game_state = get_game_state(msg);
      // START signal
      if (game_state == GameState::start) {
        tree.reset();
      }
      // PLAYING signal
      else if (game_state == GameState::playing) {
        Serial.println("entered playing state");
        tree.set_beacon_state(get_next_beacon(msg));
      }
      // WON signal
      else if (game_state == GameState::won) {
        tree.state = TreeState::win_global;
      }
      // LOST signal
      else if (game_state == GameState::lost) {
        Serial.println("entered lost state");
        tree.state = TreeState::fail;
      }
    }
    // else {
    //   Serial.print("invalid message: ");
    //   Serial.println(msg);
    // }
  }

  /*** IDLE STATE ***/
  if (tree.state == TreeState::idle) {
    double speed = 0.002;
    tree.hue = 100.0;
    tree.saturation = 150 + abs(sin(time * speed * global_speed)) * 50;
    tree.value = 255 - abs(cos(time * speed)) * 55;
    tree.on_pressed([] { idle_delay.start(2000, AsyncDelay::MILLIS); });
    tree.while_pressed([] {
      double speed = 0.01;
      tree.value = 255 - abs(cos(time * speed * global_speed)) * 100;
      if (idle_delay.isExpired()) {
        tree.reset_button();
      }
    });
    tree.show();
  }

  /*** BEACON STATE ***/
  else if (tree.state == TreeState::beacon) {
    tree.while_not_pressed([] {
      double speed = 0.007;
      tree.hue = -sin(time * speed * global_speed) * 30 + 50;
      tree.value = 220 + sin(time * speed * global_speed) * 35;
      tree.saturation = 255;
    });
    tree.on_pressed([] { send_state(tree); });
    tree.while_pressed([] {
      if (tree.hue > 20) {
        tree.hue -= 0.2;
      } else {
        tree.reset_button();
        tree.state = TreeState::pressed_beacon;
      }
    });
    tree.show();
  }
  /*** PRESSED BEACON STATE ***/
  else if (tree.state == TreeState::pressed_beacon) {
    tree.hue = 20;
    tree.while_pressed([] { tree.button_was_pressed = false; });
    tree.on_pressed([] {});
    tree.show();
  }

  /*** WIN FOREST STATE ***/
  else if (tree.state == TreeState::win_forest) {
    double speed = 0.007;
    tree.hue = 135;
    tree.saturation = 140 + 20 * sin(time * speed * global_speed);
    tree.while_pressed([] { tree.button_was_pressed = false; });
    tree.on_pressed([] {});
    tree.show();
  }
  /*** WIN GLOBAL STATE ***/
  else if (tree.state == TreeState::win_global) {
    tree.hue = 240;
    tree.saturation = 170 - sin(time * 0.003 * global_speed) * 30;
    tree.while_pressed([] { tree.button_was_pressed = false; });
    tree.on_pressed([] {});
    tree.show();
  }
  /*** WAITING STATE ***/
  else if (tree.state == TreeState::waiting) {
    tree.hue = 100;
    tree.saturation = 240;
    if (tree.value > 0.2) {
      tree.value -= 0.02;
    }
    tree.while_pressed([] { tree.button_was_pressed = false; });
    tree.on_pressed([] {});
    tree.show();
  }
  /*** FAIL STATE ***/
  else if (tree.state == TreeState::fail) {
    double speed = 0.005;
    tree.hue = 2;
    tree.saturation = 240;
    tree.value = 200 - abs(cos(time * speed)) * 100;
    tree.show();
    tree.while_pressed([] { tree.button_was_pressed = false; });
    tree.on_pressed([] {});
  }
}
