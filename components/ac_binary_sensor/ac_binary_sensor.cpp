#include "ac_binary_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ac_binary_sensor {

static const char *const TAG = "ac_binary_sensor";

bool AcBinarySensorStorage::setup(InternalGPIOPin *pin) {
  this->pin = pin;
  this->pin->setup();
  this->isr_pin = this->pin->to_isr();
  this->pin->attach_interrupt(&AcBinarySensorStorage::s_gpio_intr, this, gpio::INTERRUPT_ANY_EDGE);

  return true;
}

void IRAM_ATTR HOT AcBinarySensorStorage::s_gpio_intr(AcBinarySensorStorage *storage) {
  uint32_t now = millis();
  uint32_t then = storage->last_pulse;

  uint8_t in = storage->isr_pin.digital_read();
  if (in) {
    // store timing at raising edge
    storage->last_pulse = now;
    return;
  }

  if (!storage->last_pulse) {
    // pin is high but there was no raising edge
    return;
  }

  storage->last_pulse = 0;
  if (now - then < 1000 / storage->freq / 4) {
    // falling edge but pulse too short
    return;
  }

  storage->count++;
}

void AcBinarySensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up A/C binary sensor '%s'...", this->name_.c_str());
  if (!this->storage_.setup(this->pin_)) {
    this->mark_failed();
    return;
  }
}

void AcBinarySensor::dump_config() {
  LOG_SENSOR("", "A/C Binary Sensor", this);
  LOG_PIN("  Pin: ", this->pin_);
  ESP_LOGCONFIG(TAG, "  A/C Frequency: %u Hz", this->storage_.freq);
  ESP_LOGCONFIG(TAG, "  Switching after %u pulses", this->threshold_);
  LOG_UPDATE_INTERVAL(this);
}

void AcBinarySensor::loop() {
  // ensure loop runs at (approximately) current frequency
  uint32_t now = millis();
  if (now - this->last_check_ < 1000 / this->storage_.freq) {
    return;
  }

  uint8_t count = this->storage._count;
  if (this->trace_ == count) {
    // no pulse detected
    if (this->level_ > 0) {
      if (--this->level_ == 0) {
        // bucket drained (lost too many pulses)
        // switch is expected to be open
        this->publish_state(false);
      }
    }
  } else {
    // pulse detected
    if (this->level_ < this->threshold_ * 2) {
      if (++this->level_ > this->threshold_) {
        // bucket filled to threshold
        // (detected enough pulses without draining too much)
        // switch is expected to be closed
        this->publish_state(true);
      };
    }
  }

  this->trace_ = count;
  this->last_check_ = now;
}

}  // namespace ac_binary_sensor
}  // namespace esphome
