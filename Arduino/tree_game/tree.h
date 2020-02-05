#include "FastLED.h"

enum TreeState { idle, beacon, win_local, win_global, waiting, fail };

class Tree {

public:
  char id;
  TreeState state;
  double hue;
  double saturation = 255;
  double value = 255;
  bool button_was_pressed = false;

  Tree(char tree_id) {
    id = tree_id;
    state = is_initial_beacon() ? TreeState::beacon : TreeState::idle;
  };
  bool is_initial_beacon() { return id == 'C'; }
  void set_beacon_state(char new_beacon_id) {
    if (id == new_beacon_id) {
      state = TreeState::beacon;
    }
  }
  void set_color(double h, double s, double v) {
    hue = h;
    saturation = s;
    value = v;
  }
  void show() { FastLED.showColor(CHSV(hue, saturation, value)); }
  void reset_button() { button_was_pressed = false; }

  template <typename Func> void on_pressed(Func f) {
    if (is_button_pressed() && !button_was_pressed) {
      f();
      button_was_pressed = true;
    }
  }
  template <typename Func> void while_pressed(Func f) {
    if (button_was_pressed) {
      f();
    }
  }
  template <typename Func> void while_not_pressed(Func f) {
    if (!button_was_pressed) {
      f();
    }
  }
};
