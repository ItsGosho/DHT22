#include <Arduino.h>
#include "DHT22.h"

bool oneTime = false;

#define DHT22_PIN 10

void setup() {
    Serial.begin(9600);
}

DHT22 dht22(DHT22_PIN);

void loop() {

    DHT22Measurement measurement = dht22.measure(2000);
    Serial.println(measurement.humidity);
    Serial.println(measurement.temperature);
    Serial.println(measurement.isChecksumValid);
    Serial.println(measurement.isTimedOut);
    Serial.println("------------------------>");

}