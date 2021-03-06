
#define LED_PIN 10
#define LED_COUNT 32
#define BRIGHTNESS 255

#define BUTTON_LED 11
#define BUTTON_OUT 12
#define BUTTON_IN 13

bool is_button_pressed() { return digitalRead(BUTTON_IN) == HIGH; }
bool is_valid_msg(String msg) {
  return msg.startsWith("{") && msg.endsWith("}") && msg.charAt(1) == 'Z' &&
         msg.length() == 7;
}

int get_game_state(String msg) { return msg.charAt(2) - '0'; }
char get_next_beacon(String msg) { return msg.charAt(3); }
int get_weather_state(String msg) { return msg.charAt(5) - '0'; }
