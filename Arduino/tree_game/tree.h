#include "FastLED.h"

enum TreeState { idle, beacon, win, win_global, waiting, fail };

class Tree {

public:
  String id;
  TreeState state;
  double hue;
  double saturation = 255;
  double value = 255;
  bool button_was_pressed = false;

  Tree(String tree_id) {
    id = tree_id;
    state = is_beacon() ? beacon : idle;
  };
  bool is_beacon() { return id == "C"; }
  bool matches(String tree_id) { return id == tree_id; }
  void set_color(double h, double s, double v) {
    hue = h;
    saturation = s;
    value = v;
  }

  void show() { FastLED.showColor(CHSV(hue, saturation, value)); }
  void reset_button() { button_was_pressed = false; }

  template <typename Func> void on_pressed(Func f) {
    if (is_button_pressed()) {
      f();
      button_was_pressed = true;
    }
  }

  template <typename Func> void while_pressed(Func f) {
    if (button_was_pressed) {
      f();
    }
  }
};
