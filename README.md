[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/ItsGosho/DHT22/blob/main/LICENSE)

# DHT22

Library for the DHT22 Temperature & Humidity sensor

## Simple Usage

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