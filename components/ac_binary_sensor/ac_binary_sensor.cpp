#include "ac_binary_sensor.h"

namespace esphome {
namespace ac_binary_sensor {

struct AcBinarySensorStore {
  void setup(InternalGPIOPin *pin) {
    pin->setup();
    _isr_pin = pin->to_isr();
    pin->attach_interrupt(&AcBinarySensorStore::s_gpio_intr, this, gpio::INTERRUPT_ANY_EDGE);
  }

  // interrupt handler
  static void IRAM_ATTR HOT s_gpio_intr(AcBinarySensorStore *store) {
    uint32_t now = millis();
    uint32_t then = store->_millis;

    uint8_t in = store->_isr_pin.digital_read();
    if (in) {
      // store timing at rising edge
      store->_millis = now;
      return;
    }

    if (!store->_millis) {
      // missed edge for whatever reason
      return;
    }

    if (now - then > 1000 / store->_freq / 4) {
      // count at falling edge after long enough pulse
      store->_count++;
      store->_millis = 0;
      return;
    }
  }

  volatile uint32_t _millis{0};
  volatile uint8_t _count{0};
  uint8_t _freq{0};

  ISRInternalGPIOPin _isr_pin;
};

void AcBinarySensor::setup() { storage_.setup(pin_); }

void AcBinarySensorStore::loop() {
  // ensure loop runs at (approximately) current frequency
  unit32_t now = millis();
  if (now - last_check_ < 1000 / storage_.freq) {
    return;
  }

  uint8_t count = store._count;
  if (trace_ == count) {
    // no pulse detected
    if (level_ > 0) {
      if (--level_ == 0) {
        // bucket drained (lost too many pulses)
        // switch is expected to be open
        publish_state(false);
      }
    }
  } else {
    // pulse detected
    if (level_ < threshold_ * 2) {
      if (++level_ > threshold_) {
        // bucket filled to threshold
        // (detected enough pulses without draining too much)
        // switch is expected to be closed
        publish_state(true);
      };
    }
  }

  trace_ = count;
  last_check_ = now;
}

}  // namespace ac_binary_sensor
}  // namespace esphome
