
#include "state_led.h"
#include "user_config.h"
#include "debugger.h"

// Set this to true if we use anode LED
#define RGB_STATE_LED_REVERSED true

StateLED::StateLED(IEspConfig& config) :
    RGBLed(RGB_LED_PIN_R, RGB_LED_PIN_G, RGB_LED_PIN_B, RGB_STATE_LED_REVERSED),
    _config(config),
    _blink_state(false),
    _colorTypes{
        // Normal           Colorblind
        // R    G    B      R    G    B
        {{000, 000, 000}, {000, 000, 000}}, // off
        {{063, 127, 063}, {063, 127, 063}}, // init
        {{127, 063, 127}, {127, 063, 127}}, // config 
        {{000, 127, 255}, {000, 127, 255}}, // mobile
        {{127, 255, 000}, {127, 255, 000}}, // fixed_connecting
        {{000, 255, 000}, {000, 255, 127}}, // fixed_active
        {{255, 000, 000}, {255, 127, 000}}, // fixed_error
    } {
}

void StateLED::set_color(StateLED::StateColor color) {
  DEBUG_PRINT("Changed LED to ");
  DEBUG_PRINTLN(color);
  set(_config.is_led_color_blind() ? _colorTypes[color].color_blind : _colorTypes[color].normal);
}
void StateLED::blink(StateLED::StateColor color, uint32_t frequency) {
  // Blink LED
  if(millis() % frequency > frequency / 2 && !_blink_state) {
    set_color(color);
    _blink_state = true;
  } else if(millis() % frequency < frequency / 2 && _blink_state) {
    set_color(StateLED::StateColor::off);
    _blink_state = false;
  }
}

uint8_t StateLED::get_intensity() const {
  return _config.get_led_color_intensity();
}