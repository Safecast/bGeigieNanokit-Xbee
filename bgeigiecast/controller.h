#ifndef BGEIGIECAST_CONTROLLER_HPP
#define BGEIGIECAST_CONTROLLER_HPP

#include "button.h"
#include "button_observer.h"
#include "sm_context.h"
#include "conf_server.h"
#include "bgeigie_connector.h"
#include "i_api_connector.h"
#include "i_bluetooth_connector.h"
#include "state_led.h"
#include "reporter.h"

/**
 * Main controller for the system, implements state machine to run
 */
class Controller : public Context, public ButtonObserver {
 public:
  typedef void (*sleep_fn_t)(uint32_t);

  Controller(IEspConfig& config, Stream& bGegie_connection, IApiConnector& api_connector, IBluetoohConnector& bluetooth_connector, sleep_fn_t sleep_fn = nullptr);
  virtual ~Controller() = default;

  /**
   * Set initial state for the state machine,
   */
  void setup_state_machine();

  /**
   * Initialize the controller and all of its components
   */
  void initialize();

  /**
   * Read new serial input from the bGeigie, if there is, it will report to bluetooth / api
   */
  void run_reporter();

  /**
   *
   */
  void set_reporter_outputs();

  /**
   * Go to sleep.
   * Wake up sources should be defined before sleeping
   */
  void sleep();

  /**
   * Reset and restart the system
   */
  void reset_system();

  /**
   * Callback for the button
   */
  void on_button_pressed(Button* button, uint32_t millis) override;

  /**
   * Save a state to the memory
   * @param state
   */
  void save_state(SavableState state);

  /**
   * get the saved state from the memory
   */
  SavableState get_saved_state();

  // Getters
  ConfigWebServer& get_ap_server();
  StateLED& get_state_led();
 private:
  IEspConfig& _config;
  Reporter _reporter;
  ConfigWebServer _ap_server;
  Button _mode_button;
  StateLED _state_led;

  sleep_fn_t _sleep_fn;

};

#endif //BGEIGIECAST_CONTROLLER_HPP
