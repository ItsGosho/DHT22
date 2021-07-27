[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/ItsGosho/DHT22/blob/main/LICENSE)

# DHT22

Library for the DHT22 Temperature & Humidity sensor

### Simple Usage

1. Initialize the `DHT22` class with the pin as constructor argument
2. Call the measure function, which will return you a `DHT22Measurement` object containing the
needed data

```
#include <Arduino.h>
#include "DHT22.h"

#define BAUD_RATE 9600
#define DHT22_PIN 10

DHT22 dht22(DHT22_PIN);

void setup() {
    Serial.begin(BAUD_RATE);
}

void loop() {

    DHT22Measurement measurement = dht22.measure(2000);
    Serial.println(measurement.humidity);
    Serial.println(measurement.temperature);
    Serial.println(measurement.isChecksumValid);
    Serial.println(measurement.isTimedOut);
    Serial.println("<------------------------>");

}
```

### Notes

- When power is supplied to the sensor, don't send instruction to it for the first 1 second 
to avoid unstable status
- The minimum time between measurements must be at least 2 seconds for the DHT22
- Place at least 4.7kOhm resistor between the DOUT & VCC Pin (Pull-up)
- Place 100nF capacitor between GND and VDD for wave filtering

AM2302/DHT22 Resources:
- [Used Sensor](https://www.waveshare.com/dht22-temperature-humidity-sensor.htm)
- [Data Sheet](https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf)
- [Checksum Problem](https://stackoverflow.com/questions/68547020/dht22-sensors-checksum-not-valid/68547021?noredirect=1#comment121145477_68547021)