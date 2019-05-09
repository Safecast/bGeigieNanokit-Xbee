#include "reporter.h"
#include "sm_r_init.h"
#include "sm_r_idle.h"

// Every 5 seconds
#define BGEIGIE_MEASUREMENTS_SECONDS 5
#define BGEIGIE_READING_SLACK 500
#define READING_DELAY (BGEIGIE_MEASUREMENTS_SECONDS * 1000) - BGEIGIE_READING_SLACK

Reporter::Reporter(IEspConfig& config,
                   Stream& bgeigie_connection,
                   IApiConnector& api_connector,
                   IBluetoohConnector& bluetooth_connector,
                   ReporterObserver* observer) :
    _bgeigie_connector(bgeigie_connection),
    _config(config),
    _api_connector(api_connector),
    _bluetooth(bluetooth_connector),
    _observer(observer),
    _last_reading(nullptr),
    _last_reading_moment(0),
    _report_bt(false),
    _report_api(false) {
  _api_connector.set_observer(this);
}

void Reporter::setup_state_machine() {
  set_state(new InitReporterState(*this));
}

void Reporter::set_report_output(bool bt, bool api) {
  _report_bt = bt;
  _report_api = api;
  if(_report_api) {
    _api_connector.reset();
  }
}

uint32_t Reporter::time_till_next_reading(uint32_t current) const {
  return current - _last_reading_moment < READING_DELAY ? READING_DELAY - (current - _last_reading_moment) : 0;
}

void Reporter::get_new_reading() {
  Reading* output;
  if(_bgeigie_connector.get_reading(&output) && output) {
    delete _last_reading;
    _last_reading = output;
    _last_reading_moment = millis();
    _last_reading->correctly_parsed() ? schedule_event(e_r_reading_received) : schedule_event(e_r_reading_invalid);
    if (_last_reading->get_device_id() > 0) {
      _config.set_device_id(_last_reading->get_device_id(), false);
    }
  }
}

bool Reporter::is_report_bt() const {
  return _report_bt;
}

bool Reporter::is_report_api() const {
  return _report_api;
}
bool Reporter::is_idle() const {
  return dynamic_cast<ReporterIdleState*>(get_current_state()) != nullptr;
}

void Reporter::init_bluetooth_connector() {
  if(_last_reading) {
    _bluetooth.init(_last_reading->get_device_id());
    schedule_event(e_r_bluetooth_initialized);
  }
}

void Reporter::run_bluetooth_connector() {
  _bluetooth.send_reading(_last_reading);
  schedule_event(e_r_reading_reported_bt);
}

void Reporter::init_api_connector() {
  _api_connector.init_reading_report(_last_reading);
  schedule_event(e_r_api_connector_initialized);
}

void Reporter::run_api_connector() {
  _api_connector.run();
}

void Reporter::set_observer(ReporterObserver* observer) {
  _observer = observer;
}

void Reporter::api_reported(IApiConnector::ReportApiStatus status) {
  switch(status) {
    case IApiConnector::k_report_success:
      schedule_event(e_r_reading_reported_api_success);
      break;
    case IApiConnector::k_report_skipped:
      schedule_event(e_r_reading_reported_api_no_change);
      break;
    case IApiConnector::k_report_failed:
      schedule_event(e_r_reading_reported_api_failed);
      break;
  }
}

void Reporter::report_complete(ReporterStatus status) {
  if(_observer)
    _observer->reading_reported(status);
  schedule_event(e_r_reading_reported);
}

Reading* Reporter::get_last_reading() const {
  return _last_reading;
}