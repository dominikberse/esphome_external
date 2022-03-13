# ESPHome External Components

Repository contains additional components for ESPHome.
- **ACBinarySensor**: Reliable binary sensor for A/C switches. Compatible with Luminea QS-WIFI-S04-2C, but I assume similar models will work as well.  
  *Example configuration:*
  ```
  esphome:
    name: kitchen-lights
    platform: ESP8266
    board: esp01_1m

  external_components:
    - source:
        type: git
        url: https://github.com/dominikberse/esphome_external
      components: [ ac_binary_sensor ]

  binary_sensor:
    - platform: ac_binary_sensor
      # A/C frequency is at 50Hz
      frequency: 50
      # 2 pulses to trigger (increase if working unreliable)
      threshold: 2 
      pin:
        number: GPIO12
        inverted: true
      id: switch_1
      internal: true
      on_press:
        then:
          - light.toggle: light_1
    - platform: ac_binary_sensor
      # A/C frequency is at 50Hz
      frequency: 50
      # 2 pulses to trigger (increase if working unreliable)
      threshold: 2 
      pin:
        number: GPIO13
        inverted: true
      id: switch_2
      internal: true
      on_press:
        then:
          - light.toggle: light_2

  light:
    - platform: binary
      name: "Kitchen Light"
      id: light_1
      output: relay_1
    - platform: binary
      name: "Kitchen Table Light"
      id: light_2
      output: relay_2
        
  output:
    - platform: gpio
      pin: GPIO14
      id: relay_1
    - platform: gpio
      pin: GPIO15
      id: relay_2
  ```
