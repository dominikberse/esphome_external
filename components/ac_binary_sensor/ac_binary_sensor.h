#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/binary_sensor/sensor.h"

namespace esphome {
namespace ac_binary_sensor {

struct AcBinarySensorStorage {
  void setup(InternalGPIOPin *pin);
  bool check_state(bool *state);
  static void IRAM_ATTR HOT s_gpio_intr(AcBinarySensorStorage *store);

  volatile uint32_t last_pulse{0};
  volatile uint8_t count{0};
  uint8_t freq{0};

  GPIOInternalPin *_pin;
  ISRInternalGPIOPin isr_pin;
};

class AcBinarySensor : public Component, public binary_sensor::BinarySensor {
 public:
  void set_pin(InternalGPIOPin *pin) { pin_ = pin; }
  void set_threshold(uint8_t freq) { threshold_ = threshold; }
  void set_freq(uint8_t freq) { storage_.freq = freq; }

  void setup() override;
  void loop() override;

 private:
  InternalGPIOPin *pin_;
  AcBinarySensorStorage storage_{};

  uint8_t threshold_;
  uint8_t level_{0};
  uint8_t trace_{0};
  uint32_t last_check_{0};
};

}  // namespace ac_binary_sensor
}  // namespace esphome