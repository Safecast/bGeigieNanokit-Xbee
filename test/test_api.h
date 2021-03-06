#ifndef BGEIGIE_POINTCAST_APICONNECTOR_H
#define BGEIGIE_POINTCAST_APICONNECTOR_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <StreamString.h>

#include <i_api_connector.h>

/**
 * Test class to inject in unit test controller
 */
class TestApiConnector : public IApiConnector {
 public:
  TestApiConnector(IEspConfig& config, StreamString& output) : IApiConnector(config), connected(false), output(output) {}
  virtual ~TestApiConnector() = default;

  bool start_connect(bool initial) override {
    return connected;
  }

  void stop() override {
    // OK
  }

  bool test() override {
    return connected;
  }

  bool is_connected() override {
    return connected;
  }

  // set connected state for testing
  bool connected;

 protected:

  bool time_to_send() {
    return true;
  }

  /**
   * Send a reading to the API
   * @param reading: reading to send
   * @return: true if the API call was successful
   */
  bool send_reading(Reading* reading) override {
    if(connected) {

      char reading_json[1000];
      reading->as_json(reading_json);
      output.write((uint8_t*) reading_json, strlen(reading_json));
      return true;
    }
    else {
      save_reading(reading);
      return false;
    }
  }

  StreamString& output;
};

#endif //BGEIGIE_POINTCAST_APICONNECTOR_H
