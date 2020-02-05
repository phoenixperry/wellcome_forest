
#define LED_PIN 10
#define LED_COUNT 32
#define BRIGHTNESS 255

#define BUTTON_LED 11
#define BUTTON_OUT 12
#define BUTTON_IN 13

bool is_button_pressed() { return digitalRead(BUTTON_IN) == LOW; }
bool is_valid_msg(String msg) {
  return msg.startsWith("{") && msg.endsWith("}");
}
String get_tree_id(String msg) { return String(msg.charAt(2)); }